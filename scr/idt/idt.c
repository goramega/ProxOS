#include "idt.h"
#include "terminal.h"

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void isr128(void);
extern void irq1(void);

struct idt_entry idt[256];
struct idt_ptr idtr;

void idt_set_gate(uint8_t vector, uint32_t handler, uint16_t selector, uint8_t type_attr)  {
    idt[vector].offset_low = handler & 0xFFFF;
    idt[vector].selector = selector;
    idt[vector].zero = 0;
    idt[vector].type_attr = type_attr;
    idt[vector].offset_high = (handler >> 16) & 0xFFFF;
}

void set_idt_gate(uint8_t vector, uint32_t handler, uint16_t selector, uint8_t type_attr)
{
    idt_set_gate(vector, handler, selector, type_attr);
}

static inline void idt_load(const struct idt_ptr *ptr)
{
    __asm__ volatile (
        "lidt (%0)"
        :
        : "r"(ptr)
        : "memory"
    );
}

uint8_t idt_init(void)
{
    __asm__ volatile ("cli" : : : "memory");

    // сбрассываем таблицу
    for (int i = 0; i < 256; i++) {
        idt[i] = (struct idt_entry){0};
    }
        
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint32_t)idt;
    
    idt_set_gate(0, (uint32_t)isr0, 0x10, 0x8E); // деление на ноль
    idt_set_gate(6, (uint32_t)isr6, 0x10, 0x8E); // недопустимая инструкуция
    idt_set_gate(8, (uint32_t)isr8, 0x10, 0x8E); // double fault
    idt_set_gate(13, (uint32_t)isr13, 0x10, 0x8E); // general protection fault
    idt_set_gate(14, (uint32_t)isr14, 0x10, 0x8E); // page fault
    idt_set_gate(0x21, (uint32_t)irq1, 0x10, 0x8E); // keyboard

    idt_load(&idtr);

    return 0;
}

static __attribute__((noreturn))
void kernel_panic(void)
{
    __asm__ volatile ("cli");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void exception_handler(uint32_t vector) __attribute__((noreturn));
void exception_handler(uint32_t vector)
{
    switch (vector) {
    case 0:
        terminal_writeerror("Invalid division");
        break;
    case 6:
        terminal_writeerror("Invalid opcode");
        break;
    case 8:
        terminal_writeerror("Double fault");
        break;
    case 13:
        terminal_writeerror("General Protection Fault");
        break;
    case 14:
        terminal_writeerror("Page Fault");
        break;
    default:
        terminal_writeerror("Unknown exception");
        break;
    }

    kernel_panic();
}

void keyboard_handler(void)
{
    terminal_writestring("Keyboard bro)\n");
}