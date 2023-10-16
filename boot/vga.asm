; Allow print messages before entering long mode

        section .text
        bits 32

VGA_WIDTH  equ 80
VGA_HEIGHT equ 25

global protected_vga_print
global protected_vga_set_color

; IN  = ESI: string addr
protected_vga_print:
        pusha
        call       strlen
        call       write
        popa
        ret

; IN  = DL: y
;       DH: x
; OUT = DX: Index with offset 0xB8000 at VGA buffer
getidx:
        push       ax                            ; preserve registers

        shl        dh, 1                         ; multiply by two because every entry is a word

        mov        al, VGA_WIDTH
        mul        dl
        mov        dl, al

        shl        dl, 1
        add        dl, dh
        mov        dh, 0

        pop        ax
        ret

; IN  = DL: bg color
;       DH: fg color
protected_vga_set_color:
        shl        dl, 4

        or         dl, dh
        mov        [color], dl

        ret

; IN  = AL: ASCII char
;       DL: y
;       DH: x
put_entry_at:
        pusha
        call       getidx
        mov        ebx, edx

        mov        dl, [color]
        mov        byte [0xB8000 + ebx], al
        mov        byte [0xB8001 + ebx], dl

        popa
        ret

; IN  = AL: ASCII char
putchar:
        mov        dx, [cursor_pos]

        cmp        al, 0x0A
        je         .linefeed

        call       put_entry_at

        inc        dh
        cmp        dh, VGA_WIDTH
        jne        .cursor_moved

        mov        dh, 0

        inc        dl
        cmp        dl, VGA_HEIGHT
        jne        .cursor_moved

        mov        dl, 0
        jmp        .cursor_moved

.linefeed:
        mov        dh, 0

.cursor_moved:
        mov        [cursor_pos], dx

        ret

; IN  = ESI: string addr
;       CX: string length
write:
        pusha
.loop:
        mov        al, [esi]
        call       putchar

        dec        cx
        cmp        cx, 0
        je         .done

        inc        esi
        jmp        .loop

.done:
        popa
        ret

; IN  = ESI: zero delimited string addr
; OUT = ECX: length of string
strlen:
        push       eax
        push       esi
        mov        ecx, 0 
.loop:
        mov        al, [esi]
        cmp        al, 0
        je         .done

        inc        esi
        inc        ecx

        jmp        .loop

.done:
        pop        esi
        pop        eax
        ret

        section .data

color db 0

cursor_pos:
column db 0
row db 0


