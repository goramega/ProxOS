#ifndef IDT_H
#define IDT_H

#include <stdint.h>

void exception_handler(uint32_t vector) __attribute__((noreturn));
void keyboard_handler(void);

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

uint8_t idt_init(void);
void init_idt(void);

void idt_set_gate(uint8_t vector, uint32_t handler, uint16_t selector, uint8_t type_attr);
void set_idt_gate(uint8_t vector, uint32_t handler, uint16_t selector, uint8_t type_attr);

#endif