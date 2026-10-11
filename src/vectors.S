.macro ventry idx
  .balign 0x80
  mov x0, #\idx
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
  bl exception_handler
1:
  wfe
  b 1b
