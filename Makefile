CC ?= gcc
GRUB_MKRESCUE ?= grub-mkrescue
GRUB_FILE ?= grub-file
QEMU ?= qemu-system-i386

BUILD_DIR := build
ISO_ROOT := $(BUILD_DIR)/isodir
KERNEL := $(BUILD_DIR)/myos
ISO := $(BUILD_DIR)/myos.iso

CFLAGS ?= -m32 -std=gnu11 -ffreestanding -O2 -Wall -Wextra -Werror -fno-stack-protector -fno-pie -mno-sse -mno-sse2 -mno-mmx -msoft-float -fno-tree-vectorize
LDFLAGS ?= -m32 -nostdlib -no-pie -Wl,-T,linker.ld -Wl,--build-id=none

OBJECTS := $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/terminal.o \
	$(BUILD_DIR)/idt.o $(BUILD_DIR)/idt_stub.o $(BUILD_DIR)/io.o \
	$(BUILD_DIR)/keyboard.o $(BUILD_DIR)/pic.o

.PHONY: all kernel iso check run clean

all: iso

kernel: $(KERNEL)

iso: $(ISO)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/boot.o: boot.s | $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -c $< -o $@

$(BUILD_DIR)/kernel.o: kernel.c terminal.h idt/idt.h io/io.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. -Iio -c $< -o $@

$(BUILD_DIR)/terminal.o: terminal.c terminal.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/idt.o: idt/idt.c idt/idt.h terminal.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. -c $< -o $@

$(BUILD_DIR)/io.o: io/io.c io/io.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. -Iio -c $< -o $@

$(BUILD_DIR)/keyboard.o: io/keyboard.c io/keyboard.h io/io.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. -Iio -c $< -o $@

$(BUILD_DIR)/pic.o: idt/pic.c idt/pic.h io/io.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I. -Iio -c $< -o $@

$(BUILD_DIR)/idt_stub.o: idt/isr.s | $(BUILD_DIR)
	$(CC) -m32 -ffreestanding -c $< -o $@

$(KERNEL): $(OBJECTS) linker.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

check: $(KERNEL)
	$(GRUB_FILE) --is-x86-multiboot $(KERNEL)

$(ISO): $(KERNEL) grub.cfg
	mkdir -p $(ISO_ROOT)/boot/grub
	cp $(KERNEL) $(ISO_ROOT)/boot/myos
	cp grub.cfg $(ISO_ROOT)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o $@ $(ISO_ROOT)

run: $(ISO)
	$(QEMU) -cdrom $(ISO) -D qemu.log

clean:
	rm -rf $(BUILD_DIR)
	rm -f qemu.log