mov r2, #1
loop:
	add r2, r2, r1
	sub r1, r2, r1
	add r4, r4, #1
cmp r4, #10
bne loop
sub sp, sp, #4
str r1, [sp]
swi 0x11