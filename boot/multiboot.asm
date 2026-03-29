; ============================================================
;  multiboot.asm — Multiboot2 entry + الانتقال إلى 64-bit Long Mode
;  يعمل هكذا:
;   1. GRUB يسلّم التحكم هنا بـ 32-bit protected mode
;   2. نتحقق من دعم Long Mode عبر CPUID
;   3. نبني جداول الصفحات (PML4 → PDPT → PD) بـ 2MB huge pages
;   4. نفعّل PAE ثم Long Mode ثم Paging
;   5. نقفز بـ far jump لـ 64-bit code segment
;   6. نستدعي kernel_main(magic, addr) بـ 64-bit System V ABI
; ============================================================

; ================== Multiboot2 Header ==================
section .multiboot2
align 8

header_start:
    dd 0xE85250D6                               ; magic
    dd 0                                         ; architecture: i386 protected mode
    dd header_end - header_start                 ; header length
    dd -(0xE85250D6 + 0 + (header_end - header_start))  ; checksum

    ; طلب Framebuffer
    align 8
    dw 5        ; type: framebuffer
    dw 0        ; flags
    dd 20       ; size
    dd 0        ; width  (0 = أي قيمة)
    dd 0        ; height (0 = أي قيمة)
    dd 32       ; depth  (bits per pixel)

    ; End tag
    align 8
    dw 0
    dw 0
    dd 8

header_end:

; ================== BSS: Page Tables ==================
; نحجز مساحة لجداول الصفحات في BSS (تبدأ صفراً تلقائياً)
section .bss
align 4096

pml4_table:     resb 4096   ; مستوى 4 (الجذر)
pdpt_table:     resb 4096   ; مستوى 3
pd_table_0:     resb 4096   ; مستوى 2 — 0GB..1GB
pd_table_1:     resb 4096   ; مستوى 2 — 1GB..2GB
pd_table_2:     resb 4096   ; مستوى 2 — 2GB..3GB
pd_table_3:     resb 4096   ; مستوى 2 — 3GB..4GB  (يغطي framebuffer 0xe0000000)

; Stack للمرحلة الأولى (32-bit)
stack_bottom:
    resb 16384              ; 16KB كافية للبداية
stack_top:

; ================== 32-bit Entry Point ==================
section .text
bits 32

global _start
extern kernel_main

_start:
    cli

    ; حفظ قيم Multiboot2 (eax = magic, ebx = addr)
    ; سنحتاجهم لاحقاً عند استدعاء kernel_main
    mov edi, eax            ; magic  → edi (arg1 في 64-bit ABI لاحقاً)
    mov esi, ebx            ; addr   → esi (arg2 في 64-bit ABI لاحقاً)

    ; إعداد stack
    mov esp, stack_top

    ; --- تحقق من دعم Long Mode ---
    call check_cpuid
    call check_long_mode

    ; --- بناء جداول الصفحات ---
    call setup_page_tables

    ; --- تفعيل PAE (Physical Address Extension) ---
    mov eax, cr4
    or  eax, 1 << 5         ; CR4.PAE = 1
    mov cr4, eax

    ; --- تحميل PML4 في CR3 ---
    mov eax, pml4_table
    mov cr3, eax

    ; --- تفعيل Long Mode في EFER MSR ---
    mov ecx, 0xC0000080     ; EFER MSR
    rdmsr
    or  eax, 1 << 8         ; EFER.LME = 1
    wrmsr

    ; --- تفعيل Paging (وهو ما يُكمل تفعيل Long Mode) ---
    mov eax, cr0
    or  eax, 1 << 31        ; CR0.PG = 1
    mov cr0, eax

    ; --- تحميل GDT يدعم 64-bit ---
    lgdt [gdt64.pointer]

    ; --- القفز إلى 64-bit code ---
    ; far jump يغير CS إلى الـ 64-bit code descriptor
    jmp gdt64.code_seg : long_mode_start

; ===== دالة: التحقق من دعم CPUID =====
check_cpuid:
    ; نتحقق إذا يمكن تغيير الـ ID flag في EFLAGS
    pushfd
    pop  eax
    mov  ecx, eax
    xor  eax, 1 << 21       ; نقلب ID bit
    push eax
    popfd
    pushfd
    pop  eax
    push ecx
    popfd
    cmp  eax, ecx
    je   .no_cpuid
    ret
.no_cpuid:
    hlt                     ; إذا لا يدعم CPUID نتوقف

; ===== دالة: التحقق من دعم Long Mode =====
check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb  .no_long_mode       ; إذا لا يدعم extended CPUID

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29       ; LM bit
    jz  .no_long_mode
    ret
.no_long_mode:
    hlt

; ===== دالة: بناء جداول الصفحات =====
; نبني identity mapping لكامل الـ 4GB الأولى باستخدام 2MB huge pages
; هذا ضروري لأن framebuffer في VirtualBox يقع عند 0xe0000000 (≈3.5GB)
setup_page_tables:
    ; PML4[0] → PDPT  (يغطي أول 512GB كاملة)
    mov eax, pdpt_table
    or  eax, 0b11
    mov [pml4_table], eax

    ; PDPT[0] → pd_table_0  (0GB .. 1GB)
    mov eax, pd_table_0
    or  eax, 0b11
    mov [pdpt_table + 0 * 8], eax

    ; PDPT[1] → pd_table_1  (1GB .. 2GB)
    mov eax, pd_table_1
    or  eax, 0b11
    mov [pdpt_table + 1 * 8], eax

    ; PDPT[2] → pd_table_2  (2GB .. 3GB)
    mov eax, pd_table_2
    or  eax, 0b11
    mov [pdpt_table + 2 * 8], eax

    ; PDPT[3] → pd_table_3  (3GB .. 4GB — يغطي 0xe0000000)
    mov eax, pd_table_3
    or  eax, 0b11
    mov [pdpt_table + 3 * 8], eax

    ; ملء pd_table_0: إدخال i يغطي العنوان (i * 2MB)
    mov ecx, 0
.map_pd0:
    mov eax, ecx
    shl eax, 21
    or  eax, 0b10000011
    mov [pd_table_0 + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_pd0

    ; ملء pd_table_1: إدخال i يغطي (1GB + i * 2MB)
    mov ecx, 0
.map_pd1:
    mov eax, ecx
    shl eax, 21
    add eax, 0x40000000         ; + 1GB
    or  eax, 0b10000011
    mov [pd_table_1 + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_pd1

    ; ملء pd_table_2: إدخال i يغطي (2GB + i * 2MB)
    mov ecx, 0
.map_pd2:
    mov eax, ecx
    shl eax, 21
    add eax, 0x80000000         ; + 2GB
    or  eax, 0b10000011
    mov [pd_table_2 + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_pd2

    ; ملء pd_table_3: إدخال i يغطي (3GB + i * 2MB)
    ; هذا يغطي 0xc0000000..0xffffffff — يشمل framebuffer 0xe0000000
    mov ecx, 0
.map_pd3:
    mov eax, ecx
    shl eax, 21
    add eax, 0xc0000000         ; + 3GB
    or  eax, 0b10000011
    mov [pd_table_3 + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_pd3

    ret

; ================== GDT لـ 64-bit ==================
section .rodata

gdt64:
    dq 0                            ; Null descriptor (إلزامي)

.code_seg: equ $ - gdt64
    ; Code descriptor: L=1 (64-bit), P=1, S=1, Type=0xA (execute/read)
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)

.data_seg: equ $ - gdt64
    ; Data descriptor: P=1, S=1, Type=0x2 (read/write), DB=1
    dq (1 << 41) | (1 << 44) | (1 << 47) | (1 << 54)

.pointer:
    dw $ - gdt64 - 1        ; limit
    dq gdt64                ; base

; ================== 64-bit Code ==================
section .text
bits 64

long_mode_start:
    ; تحديث segment registers لتستخدم data descriptor الجديد
    mov ax, gdt64.data_seg
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; نستدعي kernel_main(uint32_t magic, uint32_t addr)
    ; System V AMD64 ABI: arg1=rdi، arg2=rsi
    ; edi و esi محفوظان من _start بالأعلى
    ; (نُصفّر الجزء العلوي 32-bit بأمان لأن multiboot يعطي قيم 32-bit)
    mov rdi, rdi            ; magic (موجود أصلاً)
    mov rsi, rsi            ; addr  (موجود أصلاً)

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
