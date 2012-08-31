MEMORY
{
kseg1_program_ram       (wrx)   : ORIGIN = 0xA0001000, LENGTH = 0x1000
kseg1_data_mem          (w!x)   : ORIGIN = 0xA1001000, LENGTH = 0x1000
kseg0_program_mem               : ORIGIN = 0x9D004A00, LENGTH = 0x3600
 
}
 
 
ENTRY(run)

SECTIONS
{
  .text :
  {
    *(.text*)
    *(.rodata*)
  } > kseg1_program_ram
}
 
