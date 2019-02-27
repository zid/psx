.section .text
.set softfloat
.ascii "PS-X EXE"
.byte 0, 0, 0, 0, 0, 0, 0, 0
.word main
.word 0
.word 0x80010000
.word image_size
.word 0,0,0,0
.word 0x8001FFF0
.word 0,0,0,0,0,0
.ascii "Sony Computer Entertainment Inc. for zid"
