#include "مؤشر_الفأرة_الافتراضية.h"
#include "../../../محرك_المقاطعات/مكتبة_واجهة_الإدخال_والإخراج.h"
#include "../../../محرك_الفيديو/محرك_الفيديو.h"

int mouse_cycle = 0;
int8_t mouse_byte[3];

int mouse_x = 100;
int mouse_y = 100;
int mouse_left = 0;

void mouse_init() {

    // تفعيل الماوس
    outb(0x64, 0xA8);

    // تفعيل IRQ12
    outb(0x64, 0x20);
    uint8_t status = inb(0x60);
    status |= 2;
    outb(0x64, 0x60);
    outb(0x60, status);

    // إرسال أمر البدء
    outb(0x64, 0xD4);
    outb(0x60, 0xF4);
}

void mouse_handler() {

    uint8_t status = inb(0x64);

    if (!(status & 0x20)) {
        outb(0xA0, 0x20);
        outb(0x20, 0x20);
        return;
    }

    uint8_t data = inb(0x60);

    mouse_byte[mouse_cycle++] = data;

    if (mouse_cycle == 1) {
        if (!(mouse_byte[0] & 0x08)) {
            mouse_cycle = 0;
        }
    }

    if (mouse_cycle == 3) {

        int dx = (int8_t)mouse_byte[1];
        int dy = (int8_t)mouse_byte[2];

        mouse_x += dx;
        mouse_y -= dy;
    // ✅ قراءة زر الماوس
        mouse_left = mouse_byte[0] & 0x1;

        // ✅ منع الخروج من الشاشة
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_y < 0) mouse_y = 0;

        if (mouse_x > (int)fb_get_width() - 16)
            mouse_x = fb_get_width() - 16;

        if (mouse_y > (int)fb_get_height() - 16)
            mouse_y = fb_get_height() - 16;

        mouse_cycle = 0;
    }

    outb(0xA0, 0x20);
    outb(0x20, 0x20);
}


static const uint8_t cursor[16][16] = {
{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{2,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0},
{2,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0},
{2,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0},
{2,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0},
{2,1,1,1,1,1,2,0,0,0,0,0,0,0,0,0},
{2,1,1,1,1,1,1,2,0,0,0,0,0,0,0,0},
{2,1,1,1,1,1,1,1,2,0,0,0,0,0,0,0},
{2,1,1,1,1,1,2,2,0,0,0,0,0,0,0,0},
{2,1,1,1,2,1,2,0,0,0,0,0,0,0,0,0},
{2,1,1,2,0,2,0,0,0,0,0,0,0,0,0,0},
{2,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0},
{2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};


void draw_cursor() {
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {

            int px = mouse_x + x;
            int py = mouse_y + y;

            // ✅ تم إضافة (uint32_t) لحل مشكلة التحذير وضمان أمان المقارنة
            if (px < 0 || py < 0 ||
                (uint32_t)px >= fb_get_width() || 
                (uint32_t)py >= fb_get_height())
                continue;

            uint8_t p = cursor[y][x];

            if (p == 1) {
                fb_putpixel(px, py, 0xFFFFFF); // أبيض
            } 
            else if (p == 2) {
                fb_putpixel(px, py, 0x000000); // أسود
            }
            // 0 = شفاف → لا ترسم
        }
    }
}

