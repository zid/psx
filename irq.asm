.section .data
regsave:
	.space 128,0

.section .text, "ax", @progbits
.align 2
.global irq_install
.extern irq_callback
.set noat

irq_install:
	lui  $v0, 0x8001
	addiu $k0, $v0, except_vect
	lui $v0, 0x8000
	addiu $v0, $v0, 0x80

/* Ghetto copy except_vect to 0x80000080 */
	lw $v1, 0($k0)
	sw $v1, 0($v0)

	lw $v1, 4($k0)
	sw $v1, 4($v0)
	lw $v1, 8($k0)
	sw $v1, 8($v0)

/* Set IRQ flag on COP0 */
	addiu $v0, $0, 1029 
	mtc0 $v0, $12

/* Return with IRQs enabled */
	rfe
	jr $ra

save_regs_and_callback:
	la $k0, regsave
	sw $at, 0x0($k0)
	sw $v0, 0x4($k0)
	sw $v1, 0x08($k0)
	sw $a0, 0x0c($k0)
	sw $a1, 0x10($k0)
	sw $a2, 0x14($k0)
	sw $a3, 0x18($k0)
	sw $t0, 0x1c($k0)
	sw $t1, 0x20($k0)
	sw $t2, 0x24($k0)
	sw $t3, 0x28($k0)
	sw $t4, 0x2c($k0)
	sw $t5, 0x30($k0)
	sw $t6, 0x34($k0)
	sw $t7, 0x38($k0)
	sw $s0, 0x3c($k0)
	sw $s1, 0x40($k0)
	sw $s2, 0x44($k0)
	sw $s3, 0x48($k0)
	sw $s4, 0x4c($k0)
	sw $s5, 0x50($k0)
	sw $s6, 0x54($k0)
	sw $s7, 0x58($k0)
	sw $t8, 0x5c($k0)
	sw $t9, 0x60($k0)
	sw $k0, 0x64($k0)
	sw $k1, 0x68($k0)
	sw $gp, 0x6c($k0)
	sw $sp, 0x70($k0)
	sw $fp, 0x74($k0)
	sw $ra, 0x78($k0)

	jal irq_callback

	lw $at, 0x00($k0)
	lw $v0, 0x04($k0)
	lw $v1, 0x08($k0)
	lw $a0, 0x0c($k0)
	lw $a1, 0x10($k0)
	lw $a2, 0x14($k0)
	lw $a3, 0x18($k0)
	lw $t0, 0x1c($k0)
	lw $t1, 0x20($k0)
	lw $t2, 0x24($k0)
	lw $t3, 0x28($k0)
	lw $t4, 0x2c($k0)
	lw $t5, 0x30($k0)
	lw $t6, 0x34($k0)
	lw $t7, 0x38($k0)
	lw $s0, 0x3c($k0)
	lw $s1, 0x40($k0)
	lw $s2, 0x44($k0)
	lw $s3, 0x48($k0)
	lw $s4, 0x4c($k0)
	lw $s5, 0x50($k0)
	lw $s6, 0x54($k0)
	lw $s7, 0x58($k0)
	lw $t8, 0x5c($k0)
	lw $t9, 0x60($k0)
	lw $k0, 0x64($k0)
	lw $k1, 0x68($k0)
	lw $gp, 0x6c($k0)
	lw $sp, 0x70($k0)
	lw $fp, 0x74($k0)
	lw $ra, 0x78($k0)

	mfc0 $k0, $14
	rfe
	j $k0

except_vect:
	la $k0, save_regs_and_callback
	j $k0
