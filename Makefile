CROSS   ?= aarch64-linux-gnu-
CC      := $(CROSS)gcc
LD      := $(CROSS)ld
OBJDUMP := $(CROSS)objdump

SRC    := src
BUILD  := build
KERNEL := $(BUILD)/kernel.elf

CFLAGS := -Wall -Wextra -O2 -g \
					-mcpu=cortex-a72 -mgeneral-regs-only \
          -ffreestanding -nostdlib -fno-pic -fno-pie -fno-stack-protector \
          -MMD -MP

C_SRCS := $(wildcard $(SRC)/*.c)
S_SRCS := $(wildcard $(SRC)/*.S)
OBJS   := $(C_SRCS:$(SRC)/%.c=$(BUILD)/%.c.o) $(S_SRCS:$(SRC)/%.S=$(BUILD)/%.S.o)
DEPS   := $(OBJS:.o=.d)

QEMU := qemu-system-aarch64
QEMU_FLAGS := -machine virt -cpu cortex-a72 -nographic -kernel $(KERNEL)

.PHONY: all run debug dump clean

all: $(KERNEL)

$(KERNEL): $(OBJS) linker.ld
	$(LD) -T linker.ld -o $@ $(OBJS)

$(BUILD)/%.c.o: $(SRC)/%.c
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.S.o: $(SRC)/%.S
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(KERNEL)
	$(QEMU) $(QEMU_FLAGS)

debug: $(KERNEL)
	$(QEMU) $(QEMU_FLAGS) -S -s

dump: $(KERNEL)
	$(OBJDUMP) -d $(KERNEL) | less

clean:
	rm -rf $(BUILD)

-include $(DEPS)
