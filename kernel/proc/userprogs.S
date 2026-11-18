.section .rodata
.balign 16

.global hello_elf_start
.global hello_elf_end

hello_elf_start:
  .incbin "build/user/hello.elf"
hello_elf_end:

.balign 16
.global echo_elf_start
.global echo_elf_end

echo_elf_start:
  .incbin "build/user/echo.elf"
echo_elf_end:

.balign 16
.global shell_elf_start
.global shell_elf_end

shell_elf_start:
  .incbin "build/user/shell.elf"
shell_elf_end:
