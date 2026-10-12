.macro ventry idx
  .balign 0x80
  sub sp, sp, #272
  stp x0, x1, [sp, #0]
  mov x1, #\idx
  b exception_entry
.endm

.section .text
.balign 0x800
.global vectors
vectors:
  ventry 0
  ventry 1
  ventry 2
  ventry 3
  ventry 4
  ventry 5
  ventry 6
  ventry 7
  ventry 8
  ventry 9
  ventry 10
  ventry 11
  ventry 12
  ventry 13
  ventry 14
  ventry 15

exception_entry:
  stp x2, x3, [sp, #16]
  stp x4, x5, [sp, #32]
  stp x6, x7, [sp, #48]
  stp x8, x9, [sp, #64]
  stp x10, x11, [sp, #80]
  stp x12, x13, [sp, #96]
  stp x14, x15, [sp, #112]
  stp x16, x17, [sp, #128]
  stp x18, x19, [sp, #144]
  stp x20, x21, [sp, #160]
  stp x22, x23, [sp, #176]
  stp x24, x25, [sp, #192]
  stp x26, x27, [sp, #208]
  stp x28, x29, [sp, #224]
  str x30, [sp, #240]

  mrs x2, elr_el1
  mrs x3, spsr_el1
  stp x2, x3, [sp, #248]

  mov x0, sp
  bl exception_handler

exception_return:
  ldp x2, x3, [sp, #248]
  msr elr_el1, x2
  msr spsr_el1, x3

  ldp x2, x3, [sp, #16]
  ldp x4, x5, [sp, #32]
  ldp x6, x7, [sp, #48]
  ldp x8, x9, [sp, #64]
  ldp x10, x11, [sp, #80]
  ldp x12, x13, [sp, #96]
  ldp x14, x15, [sp, #112]
  ldp x16, x17, [sp, #128]
  ldp x18, x19, [sp, #144]
  ldp x20, x21, [sp, #160]
  ldp x22, x23, [sp, #176]
  ldp x24, x25, [sp, #192]
  ldp x26, x27, [sp, #208]
  ldp x28, x29, [sp, #224]
  ldr x30, [sp, #240]

  ldp x0, x1, [sp, #0]

  add sp, sp, #272
  eret
