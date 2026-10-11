.section .text.boot, "ax"
.global _start

_start:
  mrs x0, mpidr_el1
  and x0, x0, #0xff
  cbnz x0, park

  ldr x0, =__stack_top
  mov sp, x0

  ldr x0, =__bss_start
  ldr x1, =__bss_end
1:
  cmp x0, x1
  b.hs 2f
  stp xzr, xzr, [x0], #16
  b 1b
2:
  adr x0, vectors
  msr vbar_el1, x0
  isb

  bl kernel_main

core0_hang:
  wfe
  b core0_hang

park:
  wfe
  b park
