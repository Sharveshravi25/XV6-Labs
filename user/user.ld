OUTPUT_ARCH(riscv)
ENTRY(main)

SECTIONS
{
  . = 0x0;

  .text : {
    *(.text .text.*)
    *(.rodata .rodata.*)
  }

  /* This forces the compiled sections to separate cleanly on 4KB edges */
  . = ALIGN(0x1000);

  .data : {
    *(.data .data.*)
  }

  .bss : {
    *(.bss .bss.*)
  }
}
