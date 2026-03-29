#include "../../النواه/محرك_التنفيذ/محرك_التنفيذ.h"
#include "../../النواه/التعريفات_الاساسية/أنظمة_الملفات/نظام_العرب/نظام_العرب.h"

static char buffer[128];
static char current_path[256] = "/";  // المسار الحالي يبدأ من الجذر

int sys_fs_mkdir(const char* path);
int sys_fs_delete(const char* path);
int sys_fs_exists(const char* path);
int sys_fs_copy(const char* src, const char* dest);

void sys_fs_list(const char* path,
                 void (*callback)(const char*));

static int strncmp(const char* a, const char* b, int n) {
    for (int i = 0; i < n; i++)
        if (a[i] != b[i])
            return 1;
    return 0;
}

static int strcmp(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i])
            return 1;
        i++;
    }
    return a[i] != b[i];
}

static int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

static void strcpy(char* dest, const char* src) {
    int i = 0;
    while (src[i]) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
}

static void strcat(char* dest, const char* src) {
    int i = strlen(dest);
    int j = 0;
    while (src[j]) {
        dest[i++] = src[j++];
    }
    dest[i] = 0;
}

// دالة لتقسيم السطر إلى كلمات - نسخة آمنة
static int parse_command(const char* cmd, char* tokens[], int max_tokens) {
    static char temp[128];  // جعلها static لتجنب تحذير dangling pointer
    int token_count = 0;
    int i = 0;
    
    // نسخ السطر
    while (cmd[i] && i < 127) {
        temp[i] = cmd[i];
        i++;
    }
    temp[i] = 0;
    
    i = 0;
    while (temp[i] && token_count < max_tokens) {
        // تخطي المسافات
        while (temp[i] == ' ') i++;
        if (!temp[i]) break;
        
        // بداية كلمة جديدة
        tokens[token_count] = &temp[i];
        token_count++;
        
        // انتقل حتى نهاية الكلمة
        while (temp[i] && temp[i] != ' ') i++;
        if (temp[i] == ' ') {
            temp[i] = 0;
            i++;
        }
    }
    
    return token_count;
}

// دالة لتوحيد المسار (إزالة // و /./ و معالجة /..)
static void normalize_path(const char* input, char* output) {
    char temp[256];
    char* parts[32];
    int part_count = 0;
    int i = 0;
    
    // نسخ المسار المدخل
    while (input[i] && i < 255) {
        temp[i] = input[i];
        i++;
    }
    temp[i] = 0;
    
    // تقسيم المسار إلى أجزاء
    i = 0;
    if (temp[0] == '/') {
        parts[part_count++] = "/";
        i = 1;
    }
    
    while (temp[i]) {
        // تخطي الـ / المتكررة
        while (temp[i] == '/') i++;
        if (!temp[i]) break;
        
        // استخراج الجزء
        char* part = &temp[i];
        while (temp[i] && temp[i] != '/') i++;
        if (temp[i] == '/') {
            temp[i] = 0;
            i++;
        }
        
        // معالجة . و ..
        if (strcmp(part, ".") == 0) {
            // تجاهل . (الدليل الحالي)
            continue;
        } else if (strcmp(part, "..") == 0) {
            // الرجوع للخلف إذا كان هناك أجزاء
            if (part_count > 1) {
                part_count--;
            }
        } else {
            // إضافة الجزء
            parts[part_count++] = part;
        }
    }
    
    // بناء المسار الموحد
    output[0] = 0;
    if (part_count == 0 || (part_count == 1 && strcmp(parts[0], "/") == 0)) {
        strcpy(output, "/");
        return;
    }
    
    for (i = 0; i < part_count; i++) {
        if (i == 0 && strcmp(parts[0], "/") == 0) {
            strcpy(output, "/");
            continue;
        }
        if (i > 0 && output[strlen(output)-1] != '/') {
            strcat(output, "/");
        }
        strcat(output, parts[i]);
    }
}

// دالة لبناء المسار الكامل
static void build_full_path(const char* input, char* full_path) {
    if (input[0] == '/') {
        // مسار مطلق
        strcpy(full_path, input);
    } else {
        // مسار نسبي
        strcpy(full_path, current_path);
        if (strcmp(current_path, "/") != 0 && input[0] != 0) {
            strcat(full_path, "/");
        }
        strcat(full_path, input);
    }
    normalize_path(full_path, full_path);
}

static void print_line(const char* name) {
    sys_write(name);
    sys_write("\n");
}

void shell_main() {

    sys_write("ArabOS Shell v1.0\n");
    sys_write("Type 'help' for commands\n\n");

    while (1) {

        // عرض المسار الحالي في الـ prompt
        sys_write(current_path);
        if (strcmp(current_path, "/") != 0) {
            sys_write("/");
        }
        sys_write("> ");

        int index = 0;

        while (1) {

            char c = sys_read();
            if (!c) continue;

            if (c == '\n') {
                buffer[index] = 0;
                sys_write("\n");
                break;
            }

            if (index < 127) {
                buffer[index++] = c;
                char s[2] = {c,0};
                sys_write(s);
            }
        }

        // تحليل الأوامر
        char* tokens[8];
        int token_count = parse_command(buffer, tokens, 8);
        
        if (token_count == 0) continue;
        
        const char* cmd = tokens[0];

        // ===== COMMANDS =====

        if (strcmp(cmd, "help") == 0) {
            sys_write("Commands:\n");
            sys_write("  help                 - Show commands\n");
            sys_write("  about                - About system\n");
            sys_write("  clear                - Clear screen\n");
            sys_write("  exit                 - Stop shell\n");
            sys_write("  restart              - Reboot system\n");
            sys_write("  shutdown             - Power off system\n");
            sys_write("  ls [path]            - List directory\n");
            sys_write("  cd [path]            - Change directory\n");
            sys_write("  pwd                  - Print working directory\n");
            sys_write("  touch <file>         - Create file\n");
            sys_write("  mkdir <dir>          - Create directory\n");
            sys_write("  rm <path>            - Delete file or empty directory\n");
            sys_write("  cp <src> <dest>      - Copy file\n\n");
        }

        else if (strcmp(cmd, "about") == 0) {
            sys_write("ArabOS MicroKernel\n");
            sys_write("Shell running in userland\n\n");
        }

        else if (strcmp(cmd, "clear") == 0) {
            sys_clear();
        }

        else if (strcmp(cmd, "exit") == 0) {
            sys_write("Shell terminated.\n");
            while (1);
        }

        else if (strcmp(cmd, "restart") == 0) {
            sys_restart();
        }

        else if (strcmp(cmd, "shutdown") == 0) {
            sys_shutdown();
        }

        else if (strcmp(cmd, "pwd") == 0) {
            sys_write(current_path);
            sys_write("\n\n");
        }

        else if (strcmp(cmd, "ls") == 0) {
            char full_path[256];
            const char* list_path = current_path;
            
            if (token_count > 1) {
                build_full_path(tokens[1], full_path);
                list_path = full_path;
            }
            
            sys_fs_list(list_path, print_line);
            sys_write("\n");
        }

        else if (strcmp(cmd, "cd") == 0) {
            char new_path[256];
            char full_path[256];
            
            if (token_count > 1) {
                build_full_path(tokens[1], full_path);
            } else {
                // cd بدون وسيط يذهب إلى الجذر
                strcpy(full_path, "/");
            }
            
            // التحقق من وجود المجلد
            if (sys_fs_exists(full_path)) {
                strcpy(new_path, full_path);
                // إزالة الـ / الزائدة في النهاية
                int len = strlen(new_path);
                if (len > 1 && new_path[len-1] == '/') {
                    new_path[len-1] = 0;
                }
                strcpy(current_path, new_path);
            } else {
                sys_write("Directory not found\n\n");
            }
        }

        else if (strcmp(cmd, "touch") == 0) {
            if (token_count < 2) {
                sys_write("Usage: touch <filename>\n\n");
            } else {
                char full_path[256];
                build_full_path(tokens[1], full_path);
                
                if (sys_fs_create(full_path))
                    sys_write("File created\n\n");
                else
                    sys_write("Failed to create file\n\n");
            }
        }
        
        else if (strcmp(cmd, "mkdir") == 0) {
            if (token_count < 2) {
                sys_write("Usage: mkdir <name>\n\n");
            } else {
                char full_path[256];
                build_full_path(tokens[1], full_path);
                
                if (sys_fs_mkdir(full_path))
                    sys_write("Directory created\n\n");
                else
                    sys_write("Failed to create directory\n\n");
            }
        }

        else if (strcmp(cmd, "rm") == 0) {
            if (token_count < 2) {
                sys_write("Usage: rm <path>\n\n");
            } else {
                char full_path[256];
                build_full_path(tokens[1], full_path);
                
                if (sys_fs_delete(full_path))
                    sys_write("Deleted successfully\n\n");
                else
                    sys_write("Failed to delete (make sure directory is empty)\n\n");
            }
        }

        else if (strcmp(cmd, "cp") == 0) {
            if (token_count < 3) {
                sys_write("Usage: cp <source> <destination>\n\n");
            } else {
                char src_full[256];
                char dest_full[256];
                
                build_full_path(tokens[1], src_full);
                build_full_path(tokens[2], dest_full);
                
                if (sys_fs_copy(src_full, dest_full))
                    sys_write("File copied successfully\n\n");
                else
                    sys_write("Failed to copy file\n\n");
            }
        }

        else {
            sys_write("Unknown command: ");
            sys_write(cmd);
            sys_write("\n\n");
        }
    }
}
