#include "مدير_التركيب_الرسومي.h"
#include "../محرك_الفيديو/محرك_الفيديو.h"
#include "../محرك_الفيديو/عارض_الصورة.h"

extern window_t windows[];
extern int window_count;

// رسم نافذة داخل الشاشة
static void blit_window(window_t* win) {
    // التأكد من أن النافذة داخل حدود الشاشة
    uint32_t screen_w = fb_get_width();
    uint32_t screen_h = fb_get_height();
    
    for (int y = 0; y < win->h; y++) {
        int screen_y = win->y + y;
        if (screen_y < 0 || screen_y >= (int)screen_h)
            continue;
            
        for (int x = 0; x < win->w; x++) {
            int screen_x = win->x + x;
            if (screen_x < 0 || screen_x >= (int)screen_w)
                continue;
                
            uint32_t color = win->buffer[y * win->w + x];
            fb_putpixel(screen_x, screen_y, color);
        }
    }
}

void compositor_render() {
    // خلفية ديناميكية حسب حجم الشاشة
    uint32_t screen_w = fb_get_width();
    uint32_t screen_h = fb_get_height();
    
    // رسم خلفية متدرجة من الأخضر إلى الأبيض
    for (uint32_t y = 0; y < screen_h; y++) {
        // حساب نسبة التدرج (0 في الأعلى، 255 في الأسفل)
        uint8_t ratio = (y * 255) / screen_h;
        
        // اللون الأخضر: R=0, G=255, B=0
        // اللون الأبيض: R=255, G=255, B=255
        // المزج: نبدأ بالأخضر ونتجه نحو الأبيض
        uint8_t red = ratio;           // يزيد من 0 إلى 255
        uint8_t green = 255;           // يبقى 255 طوال الوقت
        uint8_t blue = ratio;          // يزيد من 0 إلى 255
        
        // تجميع اللون بصيغة ARGB (32-bit)
        uint32_t color = (red << 16) | (green << 8) | blue;
        
        // رسم الصف الأفقي بالكامل
        for (uint32_t x = 0; x < screen_w; x++) {
            fb_putpixel(x, y, color);
        }
    }

    // ترتيب النوافذ (من الخلف للأمام)
    for (int i = 0; i < window_count; i++) {
        if (!windows[i].visible)
            continue;
        blit_window(&windows[i]);
    }
}
