# GDB startup script for debugging program in STM32F103 flash ROM
# $Id$

# Connect to the OpenOCD server
target remote localhost:3333 

#monitor reset halt

#monitor reset run
monitor sleep 1000

#load

# Reset target & gain control
#monitor reset halt
#monitor soft_reset_halt
#monitor wait_halt

# We can't use software breakpoints because we are running from ROM
#monitor arm7_9 force_hw_bkpts enable

# Break at beginning of main()
#continue

#info reg

monitor reset halt
break main
continue

