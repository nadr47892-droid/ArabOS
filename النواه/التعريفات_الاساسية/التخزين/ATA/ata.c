#include "ata.h"

/* ================= Ports ================= */

#define ATA_DATA       0x1F0
#define ATA_SECTOR_CNT 0x1F2
#define ATA_LBA_LOW    0x1F3
#define ATA_LBA_MID    0x1F4
#define ATA_LBA_HIGH   0x1F5
#define ATA_DRIVE      0x1F6
#define ATA_COMMAND    0x1F7
#define ATA_STATUS     0x1F7

#define ATA_CMD_READ   0x20
#define ATA_CMD_WRITE  0x30

/* ================= I/O ================= */

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

/* ================= Helpers ================= */

static void ata_wait() {
    while (inb(ATA_STATUS) & 0x80); // BSY
}

/* ================= Write ================= */

void ata_write_sector(uint32_t lba, uint8_t* buffer) {

    ata_wait();

    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_CNT, 1);

    outb(ATA_LBA_LOW,  (uint8_t)(lba));
    outb(ATA_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));

    outb(ATA_COMMAND, ATA_CMD_WRITE);

    ata_wait();

    for (int i = 0; i < 256; i++) {
        uint16_t data = buffer[i * 2] |
                        (buffer[i * 2 + 1] << 8);

        outw(ATA_DATA, data);
    }

    ata_wait();
}

/* ================= Read ================= */

void ata_read_sector(uint32_t lba, uint8_t* buffer) {

    ata_wait();

    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_CNT, 1);

    outb(ATA_LBA_LOW,  (uint8_t)(lba));
    outb(ATA_LBA_MID,  (uint8_t)(lba >> 8));
    outb(ATA_LBA_HIGH, (uint8_t)(lba >> 16));

    outb(ATA_COMMAND, ATA_CMD_READ);

    ata_wait();

    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_DATA);

        buffer[i * 2]     = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;
    }

    ata_wait();
}
