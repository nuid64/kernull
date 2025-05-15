.section .text
.code32
.global tss_flush
tss_flush:
	mov	$0x28, %ax
	ltr	%ax
	ret
