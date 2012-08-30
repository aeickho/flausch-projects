/*************************************************************************
 * Memory Regions
 *
 * Memory regions without attributes cannot be used for orphaned sections.
 * Only sections specifically assigned to these regions can be allocated
 * into these regions.
 *************************************************************************/
MEMORY
{
  xmem		       (wrx) : ORIGIN = 0xA0001000, LENGTH = 0x1000
}
ENTRY(run)

SECTIONS
{
  .text :
  {
    *(.text*)
    *(.rodata*)
  } > xmem
}

