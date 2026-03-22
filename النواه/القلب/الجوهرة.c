#include "الجوهرة.h"
#include "../محرك_الذاكرة/محرك_الذاكرة.h"
#include "../محرك_الجدولة/محرك_الجدولة.h"
#include "../محرك_العمليات/محرك_العمليات.h"
#include "../محرك_التعريفات/محرك_التعريفات.h"
#include "التعريفات_الاساسية/التخزين/ATA/ata.h"
#include "../محرك_انظمة_الملفات/محرك_انظمة_الملفات.h"
#include "../التعريفات_الاساسية/أنظمة_الملفات/نظام_العرب/نظام_العرب.h"

extern void shell_main();
void ata_write_sector(unsigned int lba, unsigned char* buffer);
void ata_read_sector(unsigned int lba, unsigned char* buffer);


void core_init() {

    memory_init();
    
    process_init();   

    scheduler_init();

    driver_init();


    console_write("Testing ATA...\n");

    unsigned char sector[512];
    unsigned char readback[512];

    for (int i = 0; i < 512; i++)
        sector[i] = 0;

    sector[0] = 'A';
    sector[1] = 'R';
    sector[2] = 'A';
    sector[3] = 'B';

    ata_write_sector(1, sector);
    ata_read_sector(1, readback);

    console_write("First byte: ");

    char c[2];
    c[0] = readback[0];
    c[1] = 0;

    console_write(c);
    console_write("\n");


    
    // تهيئة محرك أنظمة الملفات
    fs_engine_init();
    
    // تسجيل نظام العرب كالنظام الافتراضي
    fs_engine_register_driver(get_arabfs_driver());

    // الآن أي استدعاء لـ fs_engine_read سيذهب تلقائياً لنظام العرب
    char buffer[512];
    diskfs_read("test.txt", buffer);

    console_write("Reading file: ");
    console_write(buffer);
    console_write("\n");
   
}
void core_run() {

    process_t* shell = process_create(shell_main);

    scheduler_add(shell);

    scheduler_run();

}

