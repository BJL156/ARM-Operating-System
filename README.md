# ARM Operating System
[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-lightgrey.svg)](https://www.linux.org/)
[![Architecture](https://img.shields.io/badge/target-AArch64-red.svg)](https://developer.arm.com/Architectures/AArch64)

An operating system written from scratch for the AArch64 architecture. This project aims to explore low-level system programming, operating system development, and the AArch64 architecture.

The goal of this project is to be able to create user programs that can run using precompiled binaries that my own compiler and assembler produces.

The operating system is currently in its early stages, with development focused on establishing the foundations for a functional operating system that can run user programs. AArch64 was chosen as the target ISA to continue from the [ARM tool chain](https://github.com/BJL156#aarch64-toolchain-roadmap) that I have been developing.

### Features
- [x] Bootloader
- [x] UART
- [x] Exception handling
- [x] GIC
- [x] Timer
- [x] Memory functions (`memcpy`, `memmove`, `memset`)
- [x] PMM
- [x] MMU

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
- Early-stage development.
- Only can be ran on QEMU's virt platform.
