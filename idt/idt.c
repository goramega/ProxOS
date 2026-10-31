#include "idt.h"
#include "terminal.h"
#include "io/io.h"

static void print_field(const char *name, uint32_t value)
{
    terminal_writestring("\t");
    terminal_writestring(name);
    terminal_writestring(": ");
    terminal_writehex(value);
    terminal_writestring("\n");
}

static const char *exception_names[32] = {
    "Division By Zero",              // 0
    "Debug",                         // 1
    "Non Maskable Interrupt",        // 2
    "Breakpoint",                    // 3
    "Overflow",                      // 4
    "BOUND Range Exceeded",          // 5
    "Invalid Opcode",                // 6
    "Device Not Available",          // 7
    "Double Fault",                  // 8
    "Coprocessor Segment Overrun",   // 9
    "Invalid TSS",                   // 10
    "Segment Not Present",           // 11
    "Stack Segment Fault",           // 12
    "General Protection Fault",      // 13
    "Page Fault",                    // 14
    "Reserved",                      // 15
    "x87 Floating Point Exception",  // 16
    "Alignment Check",               // 17
    "Machine Check",                 // 18
    "SIMD Floating Point Exception", // 19
    "Virtualization Exception",      // 20
    "Control Protection Exception",  // 21
    "Reserved",                      // 22
    "Reserved",                      // 23
    "Reserved",                      // 24
    "Reserved",                      // 25
    "Reserved",                      // 26
    "Reserved",                      // 27
    "Hypervisor Injection Exception",// 28
    "VMM Communication Exception",   // 29
    "Security Exception",            // 30
    "Reserved"                       // 31
};

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


static inline void idt_load(const struct idt_ptr *ptr)
{
    __asm__ volatile (
        "lidt (%0)"
        :
        : "r"(ptr)
        : "memory"
    );
}

void idt_init(void)
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


}

static __attribute__((noreturn))
void kernel_panic(void)
{
    __asm__ volatile ("cli");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void exception_handler(struct interrupt_frame *frame) __attribute__((noreturn));
void exception_handler(struct interrupt_frame *frame)
{
    terminal_setcolor(VGA_COLOR_RED);
    terminal_writestring("\n");

    if(frame->vector >= 32) {
        terminal_writeerror("Unknown Interrupt\n");
    } else {
        terminal_writeerror(exception_names[frame->vector]);
        terminal_writestring("\n");
    }
    terminal_setcolor(VGA_COLOR_RED);

    print_field("Vector", frame->vector);
    print_field("EDI", frame->edi);
    print_field("ESI", frame->esi);
    print_field("EBP", frame->ebp);
    print_field("ESP at pusha", frame->esp_at_pusha);
    print_field("EBX", frame->ebx);
    print_field("EDX", frame->edx);
    print_field("ECX", frame->ecx);
    print_field("EAX", frame->eax);
    print_field("Error code", frame->err_code);
    print_field("EIP", frame->eip);
    print_field("CS", frame->cs);
    print_field("EFLAGS", frame->eflags);

    kernel_panic();
}

void keyboard_handler(void)
{
    uint8_t scancode = inb(0x60);

    terminal_writestring("Scancode: ");
    terminal_writehex(scancode);
    terminal_writestring("\n");

    outb(0x20, 0x20); /* EOI для master PIC */
}