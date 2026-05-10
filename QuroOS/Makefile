ASM = nasm
CC = gcc
LD = ld

ASMFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -O0 -I. -Iinclude
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

BUILD_DIR = build

KERNEL_C = src/kernel/kernel.c src/kernel/terminal.c src/kernel/string.c \
           src/kernel/qmatrix.c src/kernel/sysinfo.c src/kernel/qurofetch.c \
           src/kernel/memory.c src/kernel/bootsplash.c
DRIVERS_C = src/drivers/keyboard/keyboard.c src/drivers/mouse/mouse.c

C_SOURCES = $(KERNEL_C) $(DRIVERS_C)
C_OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

all: $(BUILD_DIR)/quros.elf

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/src/kernel
	mkdir -p $(BUILD_DIR)/src/drivers/keyboard
	mkdir -p $(BUILD_DIR)/src/drivers/mouse

$(BUILD_DIR)/src/boot.o: src/boot/boot.asm | $(BUILD_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/quros.elf: $(BUILD_DIR)/src/boot.o $(C_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

run: $(BUILD_DIR)/quros.elf
	qemu-system-i386 -kernel $< -m 256M -rtc base=localtime

clean:
	rm -rf $(BUILD_DIR) iso quros.iso

.PHONY: all run clean
