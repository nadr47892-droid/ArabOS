#include "مدير_الصفحات.h"
#include "محرك_الذاكرة.h"

#define TABLES 4
#define FB_PAGES 64

// ================== Index Macros ==================
#define PML4_INDEX(x) (((x) >> 39) & 0x1FF)
#define PDPT_INDEX(x) (((x) >> 30) & 0x1FF)
#define PD_INDEX(x)   (((x) >> 21) & 0x1FF)
#define PT_INDEX(x)   (((x) >> 12) & 0x1FF)

// ================== Flags ==================
#define PAGE_PRESENT  1
#define PAGE_WRITABLE 2
#define PAGE_USER     4




// 🔥 جداول ثابتة (الأهم)
static uint64_t pml4[ENTRIES] __attribute__((aligned(4096)));
static uint64_t pdpt[ENTRIES] __attribute__((aligned(4096)));
static uint64_t pd[TABLES][ENTRIES] __attribute__((aligned(4096)));

void* kmalloc_aligned(size_t size);

void paging_init(uint64_t fb_addr) {

    // تصفير
    for (int i = 0; i < ENTRIES; i++) {
        pml4[i] = 0;
        pdpt[i] = 0;
    }

    // ربط PDPT
    for (int t = 0; t < TABLES; t++) {

        for (int i = 0; i < ENTRIES; i++) {
            pd[t][i] = 0;
        }

        pdpt[t] = (uint64_t)pd[t] | PAGE_PRESENT | PAGE_WRITABLE;
    }

    // ربط PML4
    pml4[0] = (uint64_t)pdpt | PAGE_PRESENT | PAGE_WRITABLE;

    // Identity Mapping (4GB)
    for (int t = 0; t < TABLES; t++) {
        for (int i = 0; i < ENTRIES; i++) {

            uint64_t addr =
                ((uint64_t)t * 0x40000000ULL) +
                ((uint64_t)i * 0x200000ULL);

            pd[t][i] = addr | PAGE_PRESENT | PAGE_WRITABLE | PAGE_HUGE;
        }
    }

    // Framebuffer Mapping
    uint64_t fb_base = fb_addr & ~0x1FFFFF;

    for (int i = 0; i < FB_PAGES; i++) {

        uint64_t addr = fb_base + (i * 0x200000);

        uint64_t pd_index   = (addr >> 21) & 0x1FF;
        uint64_t pdpt_index = (addr >> 30) & 0x1FF;

        uint64_t* target_pd = (uint64_t*)(pdpt[pdpt_index] & ~0xFFF);

        target_pd[pd_index] =
            addr | PAGE_PRESENT | PAGE_WRITABLE | PAGE_HUGE;
    }
}

void paging_enable(uint64_t* new_pml4) {
    asm volatile("mov %0, %%cr3" :: "r"(new_pml4));
}

uint64_t* paging_get_pml4() {
    return pml4;
}



void map_page(uint64_t virt, uint64_t phys) {

    uint64_t* pml4 = paging_get_pml4();

    // -------- PML4 --------
    if (!(pml4[PML4_INDEX(virt)] & PAGE_PRESENT)) {

        uint64_t* new_pdpt = (uint64_t*) kmalloc_aligned(4096);

        for (int i = 0; i < 512; i++) new_pdpt[i] = 0;

        pml4[PML4_INDEX(virt)] =
            (uint64_t)new_pdpt | PAGE_PRESENT | PAGE_WRITABLE;
    }

    uint64_t* pdpt =
        (uint64_t*)(pml4[PML4_INDEX(virt)] & ~0xFFF);

    // -------- PDPT --------
    if (!(pdpt[PDPT_INDEX(virt)] & PAGE_PRESENT)) {

        uint64_t* new_pd = (uint64_t*) kmalloc_aligned(4096);

        for (int i = 0; i < 512; i++) new_pd[i] = 0;

        pdpt[PDPT_INDEX(virt)] =
            (uint64_t)new_pd | PAGE_PRESENT | PAGE_WRITABLE;
    }

    uint64_t* pd =
        (uint64_t*)(pdpt[PDPT_INDEX(virt)] & ~0xFFF);

    // -------- PD --------
    if (!(pd[PD_INDEX(virt)] & PAGE_PRESENT)) {

        uint64_t* new_pt = (uint64_t*) kmalloc_aligned(4096);

        for (int i = 0; i < 512; i++) new_pt[i] = 0;

        pd[PD_INDEX(virt)] =
            (uint64_t)new_pt | PAGE_PRESENT | PAGE_WRITABLE;
    }

    uint64_t* pt =
        (uint64_t*)(pd[PD_INDEX(virt)] & ~0xFFF);

    // -------- PT --------
    pt[PT_INDEX(virt)] =
        (phys & ~0xFFF) | PAGE_PRESENT | PAGE_WRITABLE;
}


void* kmalloc_aligned(size_t size) {
    void* ptr = kmalloc(size + 4096);

    uint64_t addr = (uint64_t)ptr;

    addr = (addr + 0xFFF) & ~0xFFF; // align 4KB

    return (void*)addr;
}
