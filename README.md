# SakanaOS

A toy i386 OS kernel written in C and NASM assembly, booted through GRUB multiboot, run under QEMU emulation.

## Requirements

Example for Ubuntu / Debian:

```bash
sudo apt update
sudo apt install clang nasm binutils grub-common xorriso mtools qemu-system-x86 gdb
```

## Quick start

```bash
make            # build kernel.elf
make run        # boot the kernel under QEMU
```

Once boot finishes, the screen shows a `>` prompt. Type a command directly in the QEMU window and press Enter, for example:

```text
> help
> memory
> atawrite 0 helloworld
> ataread 0 10
read: helloworld
```

## Supported commands

After boot the kernel enters its built-in shell, see [shell.c](kernel/src/shell/shell.c). It currently supports these commands:

| Command | Usage | Description |
| --- | --- | --- |
| `help` | `help` | List the names of all registered commands |
| `clear` | `clear` | Clear the screen through the VGA driver |
| `message` | `message` | Print the kernel's internal log buffer, covering the INFO, WARN, and ERROR levels |
| `memory` | `memory` | Show whether the heap is enabled, plus the size and free status of each heap block and total memory used |
| `ataread` | `ataread <addr> <size>` | Read `size` bytes starting at `addr` from the first storage device found, and print it as a string |
| `atawrite` | `atawrite <addr> <string>` | Write `string` to `addr` on the storage device |
| `int` | `int <n>` | Call `isr_interrupt` to manually trigger interrupt number `n`, useful for testing the IDT/ISR |
