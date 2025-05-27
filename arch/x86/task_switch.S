.text

.global task_switch
task_switch:
	movl	4(%esp), %eax
	movl	8(%esp), %esp
	jmp	*%eax

.global read_eip
read_eip:
	pop	%eax
	jmp	*%eax
