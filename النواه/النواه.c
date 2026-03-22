#include <stdint.h>
#include "محرك_الفيديو/محرك_الفيديو.h"
#include "محرك_الفيديو/عارض_الصورة.h"
#include "القلب/الجوهرة.h"

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

/*
 * الآن kernel_main تُستدعى من 64-bit long mode
 * المعاملات تأتي عبر System V AMD64 ABI:
 *   rdi = magic (كانت eax في Multiboot2)
 *   rsi = addr  (كانت ebx في Multiboot2)
 * نستخدم uint64_t للعناوين لضمان التوافق مع 64-bit
 */

struct multiboot2_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot2_tag_framebuffer {
    uint32_t type;
    uint32_t size;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint16_t reserved;
};

/* في 64-bit نستخدم uint64_t للعناوين */
void kernel_main(uint64_t magic, uint64_t addr)
{
    if ((uint32_t)magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        while (1);
    }

    /* addr هو عنوان 64-bit الآن — نتعامل معه مباشرة */
    uint8_t* ptr = (uint8_t*)addr;
    uint32_t total_size = *(uint32_t*)ptr;

    ptr += 8;

    framebuffer_info_t fb_info;
    int fb_found = 0;

    char* cmdline = 0;
    int text_mode = 0;

    while (ptr < (uint8_t*)addr + total_size) {

        struct multiboot2_tag* tag = (struct multiboot2_tag*)ptr;

        if (tag->type == 0) break;  /* End tag */

        if (tag->type == 1) {   /* Command line */
            cmdline = (char*)(ptr + 8);
        }

        if (tag->type == 8) {   /* Framebuffer */
            struct multiboot2_tag_framebuffer* fb =
                (struct multiboot2_tag_framebuffer*)tag;

            fb_info.address = fb->framebuffer_addr;
            fb_info.width   = fb->framebuffer_width;
            fb_info.height  = fb->framebuffer_height;
            fb_info.pitch   = fb->framebuffer_pitch;
            fb_info.bpp     = fb->framebuffer_bpp;

            fb_found = 1;
        }

        ptr += (tag->size + 7) & ~7;
    }

    /* تحقق إذا تم اختيار text mode */
    if (cmdline) {
        char* p = cmdline;
        while (*p) {
            if (p[0]=='t' && p[1]=='e' && p[2]=='x' && p[3]=='t') {
                text_mode = 1;
                break;
            }
            p++;
        }
    }

    /* إذا تم اختيار الوضع النصي */
    if (text_mode) {
        core_init();
        core_run();
        return;
    }

    /* الوضع الرسومي */
    if (fb_found) {
        fb_init(&fb_info);
        fb_clear(0x00224488);
        draw_rect(100, 100, 200, 150, 0x00FF0000);
    }

    while (1);
}
