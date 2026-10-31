#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void keyboard_handler(void);

struct interrupt_frame {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_at_pusha; /* Сохранённый pusha ESP, не ESP до исключения */
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t vector;
    uint32_t err_code;

    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

struct idt_entry {
    uint16_t offset_low; // 0 ... 15 байт обработчика
    uint16_t selector; // селектор кода
    uint8_t zero; // постоянно 0
    uint8_t type_attr;
    uint16_t offset_high; // 16 ... 31 байт обработчика
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void idt_init(void);


void idt_set_gate(uint8_t vector, uint32_t handler, uint16_t selector, uint8_t type_attr);


void exception_handler(struct interrupt_frame *frame) __attribute__((noreturn));
void exception_handler(struct interrupt_frame *frame);

#endif