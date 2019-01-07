# My FIRST GPU assembly program
.include "vc4.qinc"

#
# The main code starts after the Definitions and Macros sections
#

############################################################
# Definitions
############################################################

.set M_FF000000, ra2
.set setup_indexed_final, rb0
.set gpuindex, ra1
.set gpuindex8, ra4
.set start_y, rb1
.set elem_offs, ra3
.set add_input_x, rb16
.set add_input_y, rb4
.set  add_nexta, ra5
.set  add_nextb, rb5
.set gpuindexjump, ra6
.set num_gpus, rb6
.set numreps, ra7
.set in_addr, ra0
.set M_00FFFFFF, ra8
.set out_addr, rb8
.set M_000000FF, ra9
.set temp_addr, rb9
.set M_FFFFFF00, ra10
.set setup_index_initial, rb10
.set in_addr2, ra11
.set offsstore, rb2
.set add_input_stride, ra31
.set add_input_height, ra29
.set add_output_stride, rb29
.set add_output_height, ra30
.set add_output_x, ra19
.set add_output_y, rb20
.set curx, ra25
.set cury, ra23
.set remx, rb24
.set remy, rb28



.macro write_full_calc
   mov vpm, rb13
   mov vpm, rb15
   mov vpm, rb17
   mov vpm, rb19
   mov vpm, rb21
   mov vpm, rb23
   mov vpm, rb25
   mov vpm, rb27
.endm

.macro write_horz_calc
   mov vpm, rb14
   mov vpm, ra16
   mov vpm, rb18
   mov vpm, ra20
   mov vpm, rb22
   mov vpm, ra24
   mov vpm, rb26
   mov vpm, ra28
.endm

.macro horz_dialate
  .rep i, 4
    mov r0, rb13+i*4
    ror r1, r0, 8   
    and r1, r1, M_00FFFFFF
    mov r2, r0<<1  
    ror r2, r2, 8
    and r2, r2, M_FF000000
    or rb12, r1, r2
    ror r1, r0, 24
    and r1, r1, M_FFFFFF00
    mov r2, r0>>1  
    ror r2, r2, 24
    and r2, r2, M_000000FF
    or rb11, r1, r2
    v8max r1, r0, rb12
    v8max rb13+i*4, r1, rb11
  
    mov r0, rb15+i*4
    ror r1, r0, 8   
    and r1, r1, M_00FFFFFF
    mov r2, r0<<1  
    ror r2, r2, 8
    and r2, r2, M_FF000000
    or rb12, r1, r2
    ror r1, r0, 24
    and r1, r1, M_FFFFFF00
    mov r2, r0>>1  
    ror r2, r2, 24
    and r2, r2, M_000000FF
    or rb11, r1, r2
    v8max r1, r0, rb12
    v8max rb15+i*4, r1, rb11

  .endr
.endm

.macro vert_dialate
   v8max r1, ra12, rb14
   v8max rb13, r1, ra16
   v8max r1, rb14, ra16
   v8max rb15, r1, rb18
   v8max r1, ra16, rb18
   v8max rb17, r1, ra20
   v8max r1, rb18, ra20
   v8max rb19, r1, rb22
   v8max r1, ra20, rb22
   v8max rb21, r1, ra24
   v8max r1, rb22, ra24
   v8max rb23, r1, rb26
   v8max r1, ra24, rb26
   v8max rb25, r1, ra28
   v8max r1, rb26, ra28
   v8max rb27, r1, rb30
.endm


.macro do_sub
    mov vw_setup, setup_indexed_final
   .rep i, 4
      mov t1s, r3
      add r3, r3, add_input_stride
      mov t0s, r3
      add r3, r3, add_input_stride
   .endr

   ldtmu1
   ldtmu0; v8subs vpm, rb13, r4 
   ldtmu1; v8subs vpm, rb15, r4 
   ldtmu0; v8subs vpm, rb17, r4
   ldtmu1; v8subs vpm, rb19, r4
   ldtmu0; v8subs vpm, rb21, r4
   ldtmu1; v8subs vpm, rb23, r4
   ldtmu0; v8subs vpm, rb25, r4
   v8subs vpm, rb27, r4 
.endm



############################################################
# Main Code
############################################################

mov M_FF000000, 0xFF000000
mov M_000000FF, 0x000000FF
mov M_00FFFFFF, 0x00FFFFFF
mov M_FFFFFF00, 0xFFFFFF00

mov in_addr, unif    			# input base address
mov out_addr, unif	 			# output base address
mov num_gpus, unif   			# number of additional gpus
mov gpuindex, unif   			# gpu index, but it's zero for the main loop

mov add_input_x, unif   		# input x start
mov add_input_y, unif			# input y start
mov add_input_stride, unif		# input x stride = m_w
mov add_input_height, unif		# input height = m_h

mov add_output_stride, unif		# output m_w
mov add_output_height, unif		# output m_h
mov add_output_x, unif			# where in the output file the bytes start 
mov add_output_y, unif		    # being written (coordinate).  Same as border

mov in_addr2, unif

# jump calculation for slave semaphore wait
   mov r1, 48			# r1 = 48
   sub r0, gpuindex, 1		# r0 = index - 1
   mul24 gpuindexjump, r0, r1		# gpuindexjump = 48*(index-1)
   nop

shl elem_offs, elem_num, 2			# elem_offsa = {0,1,2,3,4,5,6,7} * 8






#######################################
# Setup outer loop y
#######################################
mov cury, 0
mov r1, 64
sub r0, add_output_height, add_output_y
sub r0, r0, add_output_y
sub remy, r0, r1
:anotherloopy

#######################################
# Setup inner loop x
#######################################
mov curx, 0
sub r0, add_output_stride, add_output_x
sub r0, r0, add_output_x
mov r1, 56
sub remx, r0, r1
:anotherloopx



###############################################################################
# Set up the loop required for going through the entire bitmap.  This loop
# These are byte aligned.  
		
#
# this calculates where to store the result for the various threads
#
#    It stores the result in rb0 (aliased to setup_indexed).  It
# uses an index number from 1 to 7, multiplies it by 8 and then
# combines it with the vpm setup to tell the reader where to
# actually store the value in vpm memory
#
mov r0, vpm_setup(8,1,h32(0,0))
shl r3, gpuindex, 3
mov gpuindex8, r3
or setup_indexed_final, r0, r3








mov   r2, add_input_x					# tempbuf_addr initiaized to in_addr
sub   r2, r2, 4							# we want to start at the border so there's an overlap
add   r1, add_input_y, cury				# add the current y
sub   r1, r1, 1
add   r1, r1, gpuindex8					    # add the index to the input read
mul24 r0, add_input_stride, r1		    # r0 =  src.m_w * (src.m_borderh + cury);
add   r0, r0, curx						# r0 += curx;
add   r2, r2, r0						# r1 = r2 + r0
add   r2, r2, elem_offs
mov  offsstore, r2
add   r3, r2, in_addr

.rep i, 4
mov t0s, r3
add r3, r3, add_input_stride
mov t1s, r3
add r3, r3, add_input_stride
.endr

ldtmu0
ldtmu1; mov ra12, r4 
ldtmu0; mov rb14, r4 
ldtmu1; mov ra16, r4
ldtmu0; mov rb18, r4 
ldtmu1; mov ra20, r4
ldtmu0; mov rb22, r4 
mov t0s, r3
ldtmu1; mov ra24, r4
add t1s, r3, add_input_stride
ldtmu0; mov rb26, r4 
ldtmu1; mov ra28, r4
mov rb30, r4

    vert_dialate
    horz_dialate


   brr -, :sample_acq0, gpuindexjump     # jump to sample_acq0[index-1]
   nop
   nop
   nop
:sample_acq0
.rep i, 6
   mov -, srel(i+1)
   mov -, sacq(i+8)
   brr -,:sacq_done0
   nop
   nop
   nop
.endr
   mov -, srel(7)
   mov -, sacq(14)
:sacq_done0



   # finally write all of the results!
   
mov r2, offsstore
add r3, r2, add_input_stride
add r3, r3, in_addr2
   do_sub


   # tell the master we're done
      brr -, :sample_acq2, gpuindexjump     # jump to sample_acq0[index-1]
      nop
      nop
      nop
   :sample_acq2
      .rep i, 6
         mov -, srel(i+1)
         mov -, sacq(i+8)
         brr -,:sacq_done2
         nop
         nop
         nop
      .endr
      mov -, srel(7)
      mov -, sacq(14)
   :sacq_done2







#######################################
# Increment inner loop x
#######################################

mov r1, 56
or.setf r0, remx, remx					# r0 = remx;     // (set the flag too!)
add curx, curx, r1						# curx += r1;

brr.anynz -, :anotherloopx				# 
sub.setf r0, r0, r1						# r0 -= r1;
add.ifn curx, curx, r0; mov remx, r0	# if (r0<1) curx+=r0; remx=r0;
mov.ifn remx, 0							# if (r0<1) remx=0;
										# if (r0!=0) goto :anotherloopx;
#######################################
# Increment inner loop y
#######################################
mov r1, 64
or.setf r0, remy, remy					# r0 = remy;     // (set the flag too!)
add cury, cury, r1						# cury += r1;

brr.anynz -, :anotherloopy				# 
sub.setf r0, r0, r1						# r0 -= r1;
add.ifn cury, cury, r0; mov remy, r0	# if (r0<1) curx+=r0; remy=r0;
mov.ifn remy, 0							# if (r0<1) remy=0;
										# if (r0!=0) goto :anotherloopy



# exit the thread
:end

thrend
nop
nop
