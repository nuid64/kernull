.section .text
.code32

.global isr_stub_table
.extern isr_handler

/* No error code */
.macro ISR_NOERR num
.global isr\num
isr\num:
	cli
	pushl	$0
	pushl	$\num
	jmp	isr_common
.endm

/* With error code */
.macro ISR_ERR num
.global isr\num
isr\num:
	cli
	pushl	$\num
	jmp	isr_common
.endm

isr_common:
	pushl	%ds
	pushl	%es
	pushl	%fs
	pushl	%gs
	pushal

	movw	$0x10, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs

	pushl	%esp
	call	isr_handler
	addl	$4, %esp

	popal
	popl	%gs
	popl	%fs
	popl	%es
	popl	%ds

	addl	$8, %esp
	iret

/* Exceptions: 0-31, some have error code */
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

/* IRQs: 32–47, no error code */
ISR_NOERR 32
ISR_NOERR 33
ISR_NOERR 34
ISR_NOERR 35
ISR_NOERR 36
ISR_NOERR 37
ISR_NOERR 38
ISR_NOERR 39
ISR_NOERR 40
ISR_NOERR 41
ISR_NOERR 42
ISR_NOERR 43
ISR_NOERR 44
ISR_NOERR 45
ISR_NOERR 46
ISR_NOERR 47

.section .data
isr_stub_table:
	.long isr0
	.long isr1
	.long isr2
	.long isr3
	.long isr4
	.long isr5
	.long isr6
	.long isr7
	.long isr8
	.long isr9
	.long isr10
	.long isr11
	.long isr12
	.long isr13
	.long isr14
	.long isr15
	.long isr16
	.long isr17
	.long isr18
	.long isr19
	.long isr20
	.long isr21
	.long isr22
	.long isr23
	.long isr24
	.long isr25
	.long isr26
	.long isr27
	.long isr28
	.long isr29
	.long isr30
	.long isr31
	.long isr32
	.long isr33
	.long isr34
	.long isr35
	.long isr36
	.long isr37
	.long isr38
	.long isr39
	.long isr40
	.long isr41
	.long isr42
	.long isr43
	.long isr44
	.long isr45
	.long isr46
	.long isr47