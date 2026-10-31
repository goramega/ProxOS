#include "terminal.h"

#define VGA_MEMORY 0xB8000

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

static size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

static void terminal_scroll(void)
{
	for (size_t y = 1; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t source = y * VGA_WIDTH + x;
			const size_t destination = (y - 1) * VGA_WIDTH + x;
			terminal_buffer[destination] = terminal_buffer[source];
		}
	}

	for (size_t x = 0; x < VGA_WIDTH; x++) {
		terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
			vga_entry(' ', terminal_color);
	}
}

void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_clean(void)
{
    terminal_row = 0;
	terminal_column = 0;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	if (x >= VGA_WIDTH || y >= VGA_HEIGHT)
		return;

	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) 
{
	if (terminal_column >= VGA_WIDTH)
		terminal_column = 0;
	if (terminal_row >= VGA_HEIGHT)
		terminal_row = VGA_HEIGHT - 1;

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	terminal_column++;
	if (terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		terminal_row++;
		if (terminal_row == VGA_HEIGHT) {
			terminal_scroll();
			terminal_row = VGA_HEIGHT - 1;
		}
	}
}

void terminal_next_row(void)
{
	terminal_column = 0;
	terminal_row++;
	if (terminal_row >= VGA_HEIGHT) {
		terminal_scroll();
		terminal_row = VGA_HEIGHT - 1;
	}
}

void terminal_tabulate(void)
{
	for (int i = 0; i < 4; i++) {
        terminal_putchar(' ');
    }
    return;
}

void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++) {
        if(data[i] == '\n') {
            terminal_next_row();
            continue;
        } else if(data[i] == '\t') {
            terminal_tabulate();
            continue;
        }
		terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}

void terminal_writeerror(const char* data)
{
	char *error = "\nERROR: ";

	terminal_setcolor(VGA_COLOR_RED);
	terminal_write(error, strlen(error));
	terminal_write(data, strlen(data));
	terminal_setcolor(VGA_COLOR_WHITE);
}