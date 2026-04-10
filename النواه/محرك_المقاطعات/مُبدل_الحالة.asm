bits 64
global context_switch

; rdi = old process rsp pointer
; rsi = new process rsp pointer

context_switch:
    ; حفظ rsp الحالي
    mov [rdi], rsp

    ; تحميل rsp الجديد
    mov rsp, rsi

    ret
