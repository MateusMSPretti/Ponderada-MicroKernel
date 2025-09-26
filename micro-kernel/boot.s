; boot.s
; Compilar com: nasm -f elf32 boot.s -o boot.o

global start
extern kmain

; Constantes para o cabeçalho Multiboot
MB_MAGIC equ 0x1BADB002
MB_FLAGS equ 0x03
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)

section .multiboot
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM

section .text
start:
    ; Configura a pilha
    mov esp, stack_space
    
    ; Chama a função principal do kernel em C
    call kmain

    ; Se kmain retornar, entra em um loop infinito
    cli
.hang:
    hlt
    jmp .hang

section .bss
resb 8192 ; Reserva 8KB para a pilha
stack_space: