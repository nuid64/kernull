.section .text
.code32

.global _start
_start:
	cli

	movl	$stack_top, %esp
	movl	$stack_top, %ebp

	call	multiboot_check

	call	kmain
halt:
	hlt
	jmp	halt


multiboot_check:
	cmpl	$0x36d76289, %eax
	jne	multiboot_check.no_multiboot
	ret
multiboot_check.no_multiboot:
	call	error


/* Oops */
error:
	movw	$0x0F46, (0xB8000)
	movw	$0x0F4B, (0xB8002)
	cli
	hlt


.align 16
stack_bottom:
	.skip 0x4000
stack_top:
