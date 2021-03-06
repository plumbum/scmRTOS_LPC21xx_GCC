/*
//*****************************************************************
//  Command file for GNU linker
//
//  Generates appropriate mappings for objects in memory
//
//  Target mcu: AT91SAM7 (Atmel)
//  Code location: Internal flash 
//
//
//  $Revision: 202 $
//  $Date:: 2008-09-12 #$
//
//*****************************************************************
*/

ENTRY(_start)
STACK_SIZE = 0x400;

/* memory layout */
MEMORY
{
  ROM (rx)     : ORIGIN = 0x00000000, LENGTH = 0x00008000   /* 32K ROM */
  RAM (rw)     : ORIGIN = 0x40000000, LENGTH = 0x00002000   /* 8K RAM */
}

SECTIONS
{
  /*
  .vectors :
  {
    KEEP(*(.vectors))
  }  > ROM
  */

  /* first section is .text which is used for code */
  .text :
  {
    *crt0.o (.text)            /* Startup code */
    *(.text*)           /* code */

    __ctors_start = .;
    *(.init_array)
    *(.ctors)
     __ctors_end = .;
     __dtors_start = .;
     *(.dtors)
     __dtors_end = .;
    KEEP(SORT(*)(.ctors))
    KEEP(SORT(*)(.dtors))
    KEEP(SORT(*)(.init_array))

    . = ALIGN(4);

    *(.rodata*)         /* read-only data (constants) */

    *(.glue_7)
    *(.glue_7t)
  } > ROM

  . = ALIGN(4);
  _etext = .;

  /* .data section which is used for initialized data */
  .data :
  {
    _data = .;
    *(.ramfunc*)
    *(.data*)

    _edata = . ;
    _data_image = LOADADDR(.data) ;
    PROVIDE (edata = .);
  } > RAM AT > ROM

  . = ALIGN(4);

  /* .bss section which is used for uninitialized data */
  .bss: (NOLOAD) :
  {
    __bss_start = . ;
    __bss_start__ = . ;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
	__bss_end__ = . ;
  } > RAM

  . = ALIGN(4);
  PROVIDE (__bss_end = .);

  .noinit (NOLOAD) :
  {
     PROVIDE (__noinit_start = .) ;
    *(.noinit*)
     PROVIDE (__noinit_end = .) ;
     _end = . ;
     PROVIDE (__heap_start = .) ;
  } > RAM

  .stack ALIGN(256) :
  {
    . += STACK_SIZE;
    PROVIDE (_stack = .);
  } > RAM

  _end = . ;
  PROVIDE (end = .);


  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
}
