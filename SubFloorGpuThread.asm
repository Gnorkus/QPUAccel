# My FIRST GPU assembly program
.include "vc4.qinc"

#
# The main code starts after the Definitions and Macros sections
#

############################################################
# Definitions
############################################################

.set in_addr,           ra0
.set out_addr,          rb0
.set remx,              rb1
.set in_addr2,          ra1
.set gpuindex,          ra2
.set elem_offs,         rb2
.set gpuindex8,         ra3
.set add_input_stride,  rb3
.set gpujumpindex,      ra4
.set num_gpus,          rb4
.set add_input_x,       ra5
.set add_input_y,       rb5
.set cury,              ra6
.set add_input_height,  rb6
.set add_output_stride, ra7
.set add_output_x,      rb7
.set add_output_height, ra8
.set add_output_y,      rb8
.set curx,              ra9
.set remy,              rb9
.set setup_indexed_final, ra10

############################################################
# Macros
############################################################



############################################################
# Main Code
############################################################

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
mov add_output_x, unif			# output m_borderw
mov add_output_y, unif		    # being written (coordinate).  Same as border

mov in_addr2, unif				# input 2 base address

# jump calculation for slave semaphore wait
   mov r1, 48			# r1 = 48
   mov r0, gpuindex
   sub r0, r0, 1		# r0 = index - 1
   mul24 gpujumpindex, r0, r1		# gpuindexjump = 48*(index-1)
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
mov r1, 64
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








mov   r2, add_input_x				# if we want to start at the border so there's an overlap, use 4 instead of 0
add   r1, add_input_y, cury				# add the current y
#sub   r1, r1, 1
add   r1, r1, gpuindex8			        # if this is a thread, uncomment, add the index to the input read
mul24 r0, add_input_stride, r1		    # r0 =  src.m_w * (src.m_borderh + cury);
add   r0, r0, curx						# r0 += curx;
add   r2, r2, r0						# r1 = r2 + r0
add   r2, r2, elem_offs
add   r3, r2, in_addr			        # tempbuf_addr initiaized to in_addr

	# load the source
   .rep i, 4
      mov t1s, r3
      add r3, r3, add_input_stride
      mov t0s, r3
      add r3, r3, add_input_stride
   .endr
   ldtmu1
   ldtmu0; mov rb10, r4 
   ldtmu1; mov rb11, r4 
   ldtmu0; mov rb12, r4
   ldtmu1; mov rb13, r4 
   ldtmu0; mov rb14, r4
   ldtmu1; mov rb15, r4 
   ldtmu0; mov rb16, r4
   mov rb17, r4 

 



   brr -, :sample_acq0, gpujumpindex  # jump to sample_acq0[index-1]
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

add   r3, r2, in_addr2			        # tempbuf_addr initiaized to in_addr
   # load the subtrahend
   .rep i, 4
      mov t1s, r3
      add r3, r3, add_input_stride
      mov t0s, r3
      add r3, r3, add_input_stride
   .endr

   mov vw_setup, setup_indexed_final
   ldtmu1
   ldtmu0; v8subs vpm, rb10, r4 
   ldtmu1; v8subs vpm, rb11, r4 
   ldtmu0; v8subs vpm, rb12, r4
   ldtmu1; v8subs vpm, rb13, r4 
   ldtmu0; v8subs vpm, rb14, r4
   ldtmu1; v8subs vpm, rb15, r4 
   ldtmu0; v8subs vpm, rb16, r4
   v8subs vpm, rb17, r4 
	

   # tell the master we're done
      brr -, :sample_acq2, gpujumpindex  # jump to sample_acq0[index-1]
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

mov r1, 64								# for overlaps, use 56 instead of 64
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
