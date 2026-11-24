.section .rodata
.balign 16

.global fs_image_start
.global fs_image_end

fs_image_start:
  .incbin "build/fs.img"
fs_image_end:
