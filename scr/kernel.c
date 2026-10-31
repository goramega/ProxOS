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
	terminal_writestring("PIC: was initialize\n");
	
	if(idt_init() != 0) {
		terminal_writeerror("idt initialising\n");
	} else {
		terminal_writestring("IDT: was initialising\n");
	}
}