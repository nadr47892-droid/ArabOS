#ifndef DRIVER_H
#define DRIVER_H

void driver_init();

void console_write(const char* str);
void console_clear();

void ata_read_sector(unsigned int lba, unsigned char* buffer);
void ata_write_sector(unsigned int lba, unsigned char* buffer);

#endif
