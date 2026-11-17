# ARM Operating System
[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-lightgrey.svg)](https://www.linux.org/)
[![Architecture](https://img.shields.io/badge/target-AArch64-red.svg)](https://developer.arm.com/Architectures/AArch64)

An operating system written from scratch for the AArch64 architecture. This project aims to explore low-level system programming, operating system development, and the AArch64 architecture.

The goal of this project is to be able to run user programs that can run using precompiled binaries that my own compiler and assembler produces.

AArch64 was chosen as the target ISA to continue from the [ARM tool chain](https://github.com/BJL156#aarch64-toolchain-roadmap) that I have been developing.

### Features
- [x] Bootloader
- [x] UART
- [x] Kernel `printf` (`kprintf`)
- [x] Exception handling
- [x] GIC
- [x] Timer
- [x] Memory functions (`memcpy`, `memmove`, `memset`, `memcmp`)
- [x] PMM
- [x] Kernel heap (`kmalloc`, `kfree`)
- [x] MMU
- [x] Context switching and tasks
- [x] Preemptive round-robin scheduler
- [x] Task sleeping and dead task reaping
- [x] User mode (EL0) execution
- [x] System calls (`write`, `sleep`, `read`, `exit`)
- [x] User pointer validation and fault isolation

## Build
> [!NOTE]
> **Needs to be built on Linux. For Windows, use WSL. Additionally, the build process requires a GNU AArch64 cross toolchain along with QEMU for virtualization.**

Clone the repository and change into its directory:
```bash
git clone https://github.com/BJL156/ARM-Operating-System
cd ./ARM-Operating-System
```
Then build using the `Makefile`:
```bash
make run
```
This will automatically boot the OS using QEMU for virtualization.

## Current Limitations
- Currently supports only QEMU's `virt` machine.
