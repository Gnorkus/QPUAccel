# My FIRST GPU assembly program
.include "vc4.qinc"

.set M_FF000000, ra0
.set setup_indexed_final, rb0
.set index, ra1
.set start_y, rb1
.set tempbuf_addra, ra2
.set tempbuf_addrb, rb2
.set elem_offsa, ra3
.set elem_offsb, rb3
.set add_rega,   ra4
.set add_regb,   rb4
.set  add_nexta, ra5
.set  add_nextb, rb5
.set gpuindexjump, ra6
.set num_gpus, rb6
.set numreps, ra7
.set in_addr, rb7
.set M_00FFFFFF, ra8
.set out_addr, rb8
.set M_000000FF, ra9
.set temp_addr, rb9
.set M_FFFFFF00, ra10
.set setup_index_initial, rb10
.set setup_index_first, ra11
.set setup_index_last, rb11

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
#   mov vpm, ra12
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
    or rb31, r1, r2
    v8max r1, r0, rb12
    v8max rb13+i*4, r1, rb31
  
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
    or rb31, r1, r2
    v8max r1, r0, rb12
    v8max rb15+i*4, r1, rb31

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



mov M_FF000000, 0xFF000000
mov M_000000FF, 0x000000FF
mov M_00FFFFFF, 0x00FFFFFF
mov M_FFFFFF00, 0xFFFFFF00

# The only thing this program does is 
# set up the vpm register,
# wait for a semaphore, and then 
# exit.  We don't use any uniforms
# with this code

mov add_regb, 64
mov add_rega, 128
mov in_addr, unif    
mov out_addr, unif   
mov temp_addr, unif  
mov num_gpus, unif
mov index, unif
mov numreps, unif

#
# this calculates where to store the result for the various threads
#
#    It stores the result in rb0 (aliased to setup_indexed).  It
# uses an index number from 1 to 7, multiplies it by 8 and then
# combines it with the vpm setup to tell the reader where to
# actually store the value in vpm memory
#
mov r0, vpm_setup(8,1,h32(0,0))
shl r1, index, 3
nop
or setup_indexed_final, r0, r1



mov tempbuf_addra, in_addr			# tempbuf_addr initiaized to in_addr
shl elem_offsa, elem_num, 2			# elem_offsa = {0,1,2,3,4,5,6,7} * 8
shl elem_offsb, elem_num, 2			# elem_offs
nop
add tempbuf_addra, tempbuf_addra, elem_offsb
nop
add tempbuf_addrb, tempbuf_addra, add_regb
nop
mov add_regb, add_rega

sub tempbuf_addrb, tempbuf_addrb, add_rega


shl r0, add_rega, 3
sub r0, r0, add_rega
mov add_nexta, r0; mov add_nextb, r0

# 
# now, we try an experiment in loading from a memory table
# where the table is setup from code.  We do this so we can
# branch to the correct acquire.  
:loop

mov t0s, tempbuf_addrb
add tempbuf_addrb, tempbuf_addrb, add_rega
.rep i, 3
mov t1s, tempbuf_addra
add tempbuf_addra, tempbuf_addra, add_regb
mov t0s, tempbuf_addrb
add tempbuf_addrb, tempbuf_addrb, add_rega
.endr
mov t1s, tempbuf_addra
add tempbuf_addra, tempbuf_addra, add_regb

sub.setf r0, index, 7

mov ra12, 1
mov rb14, 2
mov ra16, 3
mov rb18, 4
mov ra20, 5
mov rb22, 6
mov ra24, 7
mov rb26, 8
mov ra28, 9
mov rb30, 0xa

brr.anyz -, :load9
nop
nop
nop
ldtmu0;
ldtmu1; mov ra12, r4 
ldtmu0; mov rb14, r4 
ldtmu1; mov ra16, r4
ldtmu0; mov rb18, r4 
ldtmu1; mov ra20, r4
ldtmu0; mov rb22, r4 
mov t0s, tempbuf_addrb
ldtmu1; mov ra24, r4
mov t1s, tempbuf_addra
ldtmu0; mov rb26, r4 
ldtmu1; mov ra28, r4
mov rb30, r4
brr -, :doneload
nop
nop
nop
:load9
ldtmu0;
ldtmu1; mov ra12, r4 
ldtmu0; mov rb14, r4 
ldtmu1; mov ra16, r4
ldtmu0; mov rb18, r4 
ldtmu1; mov ra20, r4
ldtmu0; mov rb22, r4 
mov t0s, tempbuf_addrb
ldtmu1; mov ra24, r4
ldtmu0; mov rb26, r4 
mov ra28, r4
mov rb30, 0
:doneload

add tempbuf_addra, tempbuf_addra, add_nextb
add tempbuf_addrb, tempbuf_addrb, add_nexta


   # wait for the master
   # each instruction is worth 8 bytes
   mov r1, 48			# r1 = 48
   sub r0, index, 1		# r0 = index - 1
   mul24 gpuindexjump, r0, r1		# gpuindexjump = 48*(index-1)
   nop

#   mov vw_setup, setup_indexed_final
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
   mov vw_setup, setup_indexed_final
   write_full_calc

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








sub.setf numreps, numreps, 1
brr.anynz -, :loop
nop
nop
nop


# exit the thread
:end
thrend
nop
nop
