#include "terminal.h"
#include "idt/idt.h"
#include "idt/pic.h"
#include "io/io.h"

/* This tutorial is intended for x86 targets; allow either 32-bit or 64-bit x86 compilers. */
#if !defined(__i386__) && !defined(__x86_64__)
#error "This tutorial needs to be compiled with an x86-compatible compiler"
#endif

void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
	terminal_writestring("TERMINAL: was initialize\n");

	pic_remap();
	terminal_writestring("PIC: was remaped\n");

	outb(0x21, 0xFD);  // замаскировать IRQ0 и прочие IRQ, разрешить IRQ1
	outb(0xA1, 0xFF);  // замаскировать IRQ на slave PIC

	idt_init();
	terminal_writestring("IDT: was init\n\t IRQ0 was masked\n");
	__asm__ volatile ("sti" : : : "memory");

	for (;;) {
		__asm__ volatile ("hlt");
	}
}