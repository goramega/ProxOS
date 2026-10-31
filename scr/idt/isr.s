.section .text
.global isr0
.global isr6
.global isr8
.global isr13
.global isr14
.global irq1
.extern exception_handler
.extern keyboard_handler

isr0:
    pusha

    pushl $0
    call exception_handler
    addl $4, %esp

    popa
    iret

isr6:
    pusha

    pushl $6
    call exception_handler
    addl $4, %esp

    popa
    iret

isr8:
    pusha

    pushl $8
    call exception_handler
    addl $4, %esp

    popa
    addl $4, %esp
    iret

isr13:
    pusha

    pushl $13
    call exception_handler
    addl $4, %esp

    popa
    addl $4, %esp
    iret

isr14:
    pusha

    pushl $14
    call exception_handler
    addl $4, %esp

    popa
    addl $4, %esp
    iret

irq1:
    pusha

    pushl $14
    call keyboard_handler
    addl $4, %esp

    popa
    addl $4, %esp
    iret


