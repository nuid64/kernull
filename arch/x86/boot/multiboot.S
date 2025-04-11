.set MB2_MAGIC,    0xE85250D6
.set MB2_ARCH,     0                                   # i386
.set MB2_LENGTH,   (mb2_header_end - mb2_header_start)
.set MB2_CHECKSUM, -(0xE85250D6 + MB2_ARCH + MB2_LENGTH)

        .section .multiboot_header

.align 8
mb2_header_start:
        .long MB2_MAGIC
        .long MB2_ARCH
        .long MB2_LENGTH
        .long MB2_CHECKSUM

/* Terminating tag */
.align 8
        .word 0
        .word 0
        .long 8
mb2_header_end:
