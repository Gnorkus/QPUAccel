# My FIRST GPU assembly program
.include "vc4.qinc"


# Load the value we want to add to the input into a register

mov ra0, 0x12345678
mov rb0, 0x01010101
mov ra1, unif
mov ra2, unif
mov rb2, 256
mov ra3, 1271

:loop
	mov vw_setup, vpm_setup(8,1,v32(0,0))
	mov vpm, ra0.unpack8ai
	mov vpm, ra0.unpack8bi
	mov vpm, ra0.unpack8ci
	mov vpm, ra0.unpack8di
	add ra0, ra0, 1

	mov vw_setup, vdw_setup_0(16,4,dma_h32(0,0))
	mov vw_setup, vdw_setup_1(0)
	mov vw_addr, ra2
	add 	ra2, ra2, rb2
	mov -, vw_wait

	sub.setf ra3, ra3, 1
brr.anynz -, :loop
nop
nop

:end
mov interrupt, 1;
thrend
nop
nop