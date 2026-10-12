# ARM Operating System
[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-lightgrey.svg)](https://www.linux.org/)
[![Architecture](https://img.shields.io/badge/target-AArch64-red.svg)](https://developer.arm.com/Architectures/AArch64)

A hobby operating system written from scratch for the AArch64 architecture. This project aims to explore low-level system programming, operating system development, and the ARM 64-bit (AArch64) architecture.

The project is currently in its early stages, with development focused on establishing the foundations for a functional operating system.

## Build
> [!NOTE]
> **Needs to be built on Linux. For Windows, use WSL.**

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
