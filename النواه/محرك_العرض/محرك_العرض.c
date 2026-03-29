#include "مدير_التركيب_الرسومي/مدير_التركيب_الرسومي.h"
#include "محرك_الفيديو/محرك_الفيديو.h"
#include "محرك_الفيديو/عارض_الصورة.h"
#include "مدير_النوافذ/مدير_اطارات_النوافذ/مدير_اطارات_النوافذ.h"
#include "التعريفات_الاساسية/مؤشرات_الفأرة/مؤشر_الفأرة_الافتراضي/مؤشر_الفأرة_الافتراضية.h"
#include "مدير_النوافذ/مدير_النوافذ.h"

void display_render() {
    static int initialized = 0;
    
    if (!initialized) {
        window_manager_init();
        initialized = 1;
    }
    
    // تحديث حجم الشاشة (للدعم الديناميكي)===== اي النظام يأخذ مقاس الشاشة تلقائ بدون أعادة ضبطة شخصيا (:
    window_reload_screen_size();

    window_manager_update();

    // رسم محتوى كل نافذة
    for (int i = 0; i < window_count; i++) {
        if (!windows[i].visible)
            continue;

        window_draw_content(&windows[i]);
        window_draw_frame(&windows[i]);
    }

    compositor_render();
    draw_cursor();
    fb_swap_buffers();
}
