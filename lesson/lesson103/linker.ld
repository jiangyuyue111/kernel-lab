
 ENTRY(_start)
 SECTIONS
 {
    . = 0x80000000;
    .text : {
        *(.text.boot)
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }

    . =ALIGN(8);
    sbss = .;
    .bss : {
        *(.sbss)
        *(.sbss.*)
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(8);
        }
    ebss = .;
    . = ALIGN(16);
    . += 0x1000;
    stack_top = .;
    . = ALIGN(4096);
    kernel_end = .;
 }
