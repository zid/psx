struct irq_cb
{
	void *a, *b, *c, *d;
};

static void bios_enter_crit(void)
{
	asm (
		"addiu $a0, $0, 1;"
		"syscall"
		:
		:
		: "$a0"
	);
}

static void bios_leave_crit(void)
{
	asm (
		"addiu $a0, $0, 2;"
		"syscall"
		::: "$a0"
	);
}

static void bios_sys_enq_intrp(struct irq_cb *cb)
{
	asm (
		"addiu $t1, $0, 2;"
		"addiu $t2, $0, 0xC0;"
		"addiu $a0, $0, 0;"
		"move $a1, %0;"
		"jalr $t2;"
		:
		: "r" (cb), "m" (*cb)
		: "$t1", "$t2", "$a1", "$a0"
	);
}

void irq_install(void (*f)(void))
{
	struct irq_cb cb;

	cb.a = 0;
	cb.b = 0;
	cb.c = f;
	cb.d = 0;

	bios_enter_crit();
	bios_sys_enq_intrp(&cb);
	bios_leave_crit();
}

