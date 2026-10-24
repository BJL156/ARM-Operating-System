ENTRY(_start)

SECTIONS
{
  . = 0x40080000;

  __text_start = .;
  .text : {
    KEEP(*(.text.boot))
    *(.text*)
  }
  . = ALIGN(4096);
  __text_end = .;

  __rodata_start = .;
  .rodata : {
    *(.rodata*)
  }
  . = ALIGN(4096);
  __rodata_end = .;

  __data_start = .;
  .data : {
    *(.data*)
  }

  .bss : ALIGN(16) {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(16);
    __bss_end = .;
  }

  .stack (NOLOAD) : ALIGN(4096) {
    __stack_bottom = .;
    . += 0x4000;
    __stack_top = .;
  }

  . = ALIGN(4096);
  __kernel_end = .;

  /DISCARD/ : {
    *(.comment)
    *(.note*)
    *(.eh_frame*)
  }
}
