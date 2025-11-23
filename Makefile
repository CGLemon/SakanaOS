SRCDIR := kernel/src
OBJDIR := kernel/obj
IOSDIR := iso
INCLUDE := kernel/include

SRCS := $(shell find $(SRCDIR) -name '*.asm') $(shell find $(SRCDIR) -name '*.s') $(shell find $(SRCDIR) -name '*.c')
OBJS := $(subst $(SRCDIR), $(OBJDIR), $(patsubst %.asm, %.o, $(patsubst %.s, %.o, $(patsubst %.c, %.o, $(SRCS))))   )
HDRS := $(shell find $(INCLUDE) -name '*.h')
KERNEL := kernel.elf

CC := /usr/bin/gcc
AS := /usr/bin/nasm
LD := /usr/bin/ld

GDB := /usr/bin/gdb
QEMU := /usr/bin/qemu-system-i386
GRUB_MKRESCUE := /usr/bin/grub-mkrescue

# CC_TARGET_TRIPLE := i386-elf
LD_TARGET_TRIPLE := elf_i386
AS_FORMAT := elf32

# CFLAGS := -target $(CC_TARGET_TRIPLE) -O2 -m32 -g -fno-pic -fno-pie -I $(INCLUDE)
CFLAGS    := -O2 -m32 -g -fno-pic -fno-pie -I $(INCLUDE)
ASFLAGS   := -f $(AS_FORMAT) -g
LDFLAGS   := -m $(LD_TARGET_TRIPLE) -T kernel/kernel.ld -nostdlib --build-id=none
# QEMU_FLAG := -d int,cpu_reset -no-reboot

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HDRS)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ -ffreestanding -fno-stack-protector

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.asm
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) $< -o $@

os.iso: $(KERNEL)
	mkdir -p $(IOSDIR)/boot/grub
	cp $< $(IOSDIR)/boot/$<
	@echo 'menuentry "os" {' >  $(IOSDIR)/boot/grub/grub.cfg
	@echo '	multiboot /boot/${KERNEL}' >> $(IOSDIR)/boot/grub/grub.cfg
	@echo '}' >> $(IOSDIR)/boot/grub/grub.cfg
	$(GRUB_MKRESCUE) -o os.iso $(IOSDIR)

debug: $(KERNEL)
	${QEMU} -S -s -kernel $(KERNEL) &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL)"

compile: clean $(KERNEL)

run: $(KERNEL)
	$(QEMU) -kernel $(KERNEL) $(QEMU_FLAG)

run-iso: os.iso
	$(QEMU) -cdrom os.iso

clean:
	rm -rf $(OBJS)
	rm -rf $(OBJDIR)
	rm -f $(KERNEL)
	rm -rf *.bin
	rm -rf isodir
