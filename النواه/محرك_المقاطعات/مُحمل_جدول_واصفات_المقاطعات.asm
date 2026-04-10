bits 64
global load_idt

load_idt:
    lidt [rdi]
    ret
