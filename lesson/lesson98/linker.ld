
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
    sbss = .;
    .bss : {
        *(.bss)
    }
    ebss = .;
 }