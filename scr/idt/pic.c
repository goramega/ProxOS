#include "pic.h"
#include "io.h"

void pic_remap(void)
{
    uint8_t master_mask = inb(0x21);
    uint8_t slave_mask = inb(0xA1);

    /* Start initialization and tell both PICs that ICW4 will follow. */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    /* Move hardware IRQ vectors away from CPU exception vectors. */
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    /* The slave PIC is connected to IRQ2 on the master PIC. */
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    /* Select 8086/88 mode. */
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    /* Keep the IRQ masks that were active before remapping. */
    outb(0x21, master_mask);
    outb(0xA1, slave_mask);
}