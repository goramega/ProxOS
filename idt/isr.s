.section .text

.global isr0
.global isr6
.global isr8
.global isr13
.global isr14
.global irq1

.extern exception_handler
.extern keyboard_handler

/* Для исключений, где процессор НЕ кладёт код ошибки. */
isr0:
    pushl $0              /* err_code */
    pushl $0              /* vector */
    jmp exception_common

isr6:
    pushl $0              /* err_code */
    pushl $6              /* vector */
    jmp exception_common

/* Для этих исключений CPU уже положил err_code на стек. */
isr8:
    pushl $8              /* vector */
    jmp exception_common

isr13:
    pushl $13             /* vector */
    jmp exception_common

isr14:
    pushl $14             /* vector */
    jmp exception_common

exception_common:
    pusha
    pushl %esp            /* Аргумент: struct interrupt_frame * */
    call exception_handler
    addl $4, %esp

    /* Обработчик noreturn; эти инструкции оставлены для полноты. */
    popa
    addl $8, %esp         /* убрать vector и err_code */
    iret

irq1:
    pusha
    call keyboard_handler
    popa
    iret