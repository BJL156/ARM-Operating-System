CROSS   ?= aarch64-linux-gnu-
CC      := $(CROSS)gcc
LD      := $(CROSS)ld
OBJDUMP := $(CROSS)objdump

BUILD  := build
KERNEL := $(BUILD)/kernel.elf

CFLAGS := -Wall -Wextra -O2 -g \
          -mcpu=cortex-a72 -mgeneral-regs-only \
          -ffreestanding -nostdlib -fno-pic -fno-pie -fno-stack-protector \
          -MMD -MP

K_C_SRCS := $(shell find kernel -name '*.c')
K_S_SRCS := $(shell find kernel -name '*.S')
K_OBJS   := $(K_C_SRCS:%.c=$(BUILD)/%.c.o) $(K_S_SRCS:%.S=$(BUILD)/%.S.o)
K_INC    := $(addprefix -I,$(sort $(dir $(K_C_SRCS) $(K_S_SRCS)))) -Iinclude

USER_CFLAGS := $(CFLAGS) -Iuser -Iinclude
USER_ELFS   := $(BUILD)/user/hello.elf $(BUILD)/user/echo.elf $(BUILD)/user/shell.elf

DEPS := $(K_OBJS:.o=.d) $(USER_ELFS:.elf=.d)

QEMU       := qemu-system-aarch64
QEMU_FLAGS := -machine virt,gic-version=2 -cpu cortex-a72 -m 128M -nographic -kernel $(KERNEL)

.PHONY: all run debug dump clean

all: $(KERNEL)

$(KERNEL): $(K_OBJS) kernel/linker.ld
	$(LD) -T kernel/linker.ld -o $@ $(K_OBJS)

$(BUILD)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(K_INC) -c $< -o $@

$(BUILD)/%.S.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(K_INC) -c $< -o $@

$(BUILD)/user/%.o: user/%.c
	@mkdir -p $(dir $@)
	$(CC) $(USER_CFLAGS) -c $< -o $@

$(BUILD)/user/%.elf: $(BUILD)/user/%.o user/user.ld
	$(LD) -T user/user.ld -z max-page-size=4096 -o $@ $<

$(BUILD)/kernel/proc/userprogs.S.o: $(USER_ELFS)

run: $(KERNEL)
	$(QEMU) $(QEMU_FLAGS)

debug: $(KERNEL)
	$(QEMU) $(QEMU_FLAGS) -S -s

dump: $(KERNEL)
	$(OBJDUMP) -d $(KERNEL) | less

clean:
	rm -rf $(BUILD)

-include $(DEPS)
