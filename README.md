# ARM Operating System
[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-lightgrey.svg)](https://www.linux.org/)
[![Architecture](https://img.shields.io/badge/target-AArch64-red.svg)](https://developer.arm.com/Architectures/AArch64)

A hobby operating system written from scratch for the AArch64 architecture. This project aims to explore low-level system programming, operating system development, and the ARM 64-bit (AArch64) architecture.

The goal of this project is to be able to create user programs that uses my own compiler and assembler to turn source files into runnable binaries.

The project is currently in its early stages, with development focused on establishing the foundations for a functional operating system. ARM was chosen as the target ISA to continue from the [ARM tool chain](https://github.com/BJL156#aarch64-toolchain-roadmap) that I have been developing.

### Features
- [x] Bootloader
  - [x] Parks all cores besides core 0
  - [x] Sets a valid stack pointer
  - [x] Zeros out bss
  - [x] Points to vector table
  - [x] Jumps into a C main kernel function
- [x] UART
  - [x] Uses QEMU's virt board memory mapped location
  - [x] Sends characters over a serial connection
- [x] Exception handling
  - [X] Exception Handling
  - [x] Timer interrupt
  - [x] GIC setup

## Build
> [!NOTE]
> **Needs to be built on Linux. For Windows, use WSL. Additionally, the build process requires a GNU AArch64 cross compiler toolchain along with QEMU for virtualization.**

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
- Built for only QEMU.
