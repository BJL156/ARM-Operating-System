.section .text
.global enter_user

enter_user:
  msr elr_el1, x0
  msr sp_el0, x1
  msr spsr_el1, xzr
  
  mov x0, x2
  mov x1, x3

  mov x2, xzr
  mov x3, xzr
  .irp reg, 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30
  mov x\reg, xzr
  .endr

  eret
