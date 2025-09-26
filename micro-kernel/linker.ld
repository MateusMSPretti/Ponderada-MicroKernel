/* linker.ld */
ENTRY(start)

SECTIONS
{
    . = 1M; /* Inicia o código em 1MB de memória */

    .text :
    {
        *(.multiboot) /* Coloca a seção multiboot primeiro */
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}