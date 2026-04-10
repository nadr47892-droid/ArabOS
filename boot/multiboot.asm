; ============================================================
;  multiboot.asm — نسخة مبسطة (Minimal Paging)
;  يغطي فقط 1GB — والباقي على C
; ============================================================

section .multiboot2
align 8

header_start:
    dd 0xE85250D6
    dd 0
    dd header_end - header_start
    dd -(0xE85250D6 + 0 + (header_end - header_start))

    align 8
    dw 5
    dw 0
    dd 20
    dd 0
    dd 0
    dd 32

    align 8
    dw 0
    dw 0
    dd 8

header_end:

; ================== BSS ==================
section .bss
align 4096

pml4_table: resb 4096
pdpt_table: resb 4096
pd_table_0: resb 4096   ; فقط 1GB

stack_bottom:
    resb 16384
stack_top:

; ================== 32-bit ==================
section .text
bits 32

global _start
extern kernel_main

_start:
    cli

    mov edi, eax
    mov esi, ebx

    mov esp, stack_top

    call check_cpuid
    call check_long_mode
    call setup_page_tables

    ; PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; CR3
    mov eax, pml4_table
    mov cr3, eax

    ; Long Mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt64.pointer]

    jmp gdt64.code_seg : long_mode_start

; ================== Checks ==================
check_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no
    ret
.no:
    hlt

check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no
    ret
.no:
    hlt

; ================== Paging ==================
setup_page_tables:

    ; PML4 → PDPT
    mov eax, pdpt_table
    or eax, 0b11
    mov [pml4_table], eax

    ; PDPT → PD (فقط أول 1GB)
    mov eax, pd_table_0
    or eax, 0b11
    mov [pdpt_table], eax

    ; Identity Mapping (0 → 1GB)
    mov ecx, 0
.map:
    mov eax, ecx
    shl eax, 21
    or eax, 0b10000011
    mov [pd_table_0 + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map

    ret

; ================== GDT ==================
section .rodata

gdt64:
    dq 0

.code_seg: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)

.data_seg: equ $ - gdt64
    dq (1 << 41) | (1 << 44) | (1 << 47) | (1 << 54)

.pointer:
    dw $ - gdt64 - 1
    dq gdt64

; ================== 64-bit ==================
section .text
bits 64

long_mode_start:

    mov ax, gdt64.data_seg
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rdi, rdi
    mov rsi, rsi

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
