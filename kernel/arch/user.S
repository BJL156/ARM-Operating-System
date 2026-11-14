.section .text
.global enter_user

enter_user:
  msr elr_el1, x0
  msr sp_el0, x1
  msr spsr_el1, xzr
  
  eret
