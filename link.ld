OUTPUT_ARCH(arm)

/* Entry Point */
ENTRY(Reset_Handler)

/* Place stack in SRAM1 */
_estack = 0x24050000;

SECTIONS
{
    /* OCTOSPI base */
    . = (0x70000000);

    __image_start = .;

    .binf_header :
    {
        /* 'BINF' magic */
        LONG(0x464e4942);
        /* Unknown, has to be 1 */
        LONG(0x00000001);
        /* Address where the image gets loaded to */
        LONG(__image_start);
        /* Address to vector table */
        LONG(__vector_table_start);
        /* Total image size */
        LONG(__image_end - __image_start);
        /* Unknown */ 
        BYTE(0);
        /* If this is loaded from a .shpac file, the .shpac path is copied here */
        __shpac_path = .;

        . = ALIGN(0x100);
    }

    .isr_vector :
    {
        /* Vector table offset must be a multiple of 0x400 */
        . = ALIGN(0x400);

        __vector_table_start = .;
        KEEP(*(.isr_vector)) /* Startup code */

        . = ALIGN(0x100);
    }

    .text :
    {
        . = ALIGN(4);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(4);
        _etext = .;        /* define a global symbols at end of code */
    }

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    }

    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    }
    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    }
    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    }

    /* used by the startup to initialize data */
    _sidata = LOADADDR(.data);
    /* do not copy any data, since we're already loaded in RAM */
    _sdata = .;
    _edata = .;

    .data : 
    {
        . = ALIGN(4);
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */

        . = ALIGN(4);
    }

    . = ALIGN(4);
    .bss :
    {
        /* This is used by the startup in order to initialize the .bss secion */
        _sbss = .; /* define a global symbol at bss start */
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    }

    __image_end = .;

    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
}
