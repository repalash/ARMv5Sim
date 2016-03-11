sub sp, sp, #44
mov r3, sp
mov r4, #0

loop:
	str r4, [r3]
	add r3, r3, #4
	add r4, r4, #1
cmp r4, #10
bne loop		

mov r3, sp
mov r4, #0

loop2:
	ldr r5, [r3]
	add r3, r3, #4
	add r4, r4, #1
	add r6, r6, r5
cmp r4, #10
bne loop2		
str r6, [r3]

swi 0x11