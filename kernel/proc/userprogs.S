.section .rodata
.balign 16

.global hello_elf_start
.global hello_elf_end

hello_elf_start:
  .incbin "build/user/hello.elf"
hello_elf_end:
