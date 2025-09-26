# Micro Kernel com Jogo da Forca

Este projeto consiste na criação de um micro kernel para a arquitetura x86 de 32 bits, desenvolvido como parte da atividade acadêmica de Sistemas Operacionais. O kernel é capaz de inicializar o ambiente, interagir com hardware básico (vídeo e teclado) e executar uma aplicação interativa: um Jogo da Forca.

O sistema é totalmente autocontido e não depende de nenhum sistema operacional subjacente para rodar, sendo executado diretamente no hardware (emulado via QEMU).

## Tecnologias e Ferramentas

* **Linguagens:** C (para a lógica do kernel e do jogo) e Assembly x86 (NASM, para o bootloader).
* **Compilador e Montador:** GCC (para C) e NASM (para Assembly).
* **Linker:** `ld` para unir os arquivos objeto e criar o binário final do kernel.
* **Bootloader:** GRUB, utilizado para carregar o kernel na memória.
* **Emulador:** QEMU (`qemu-system-i386`) para criar uma máquina virtual e executar o kernel sem a necessidade de hardware real.
* **Ferramentas de Build:** `grub-mkrescue`, `xorriso`, e `mtools` para a criação da imagem ISO bootável.

## Arquitetura e Componentes

O projeto é dividido em componentes distintos que trabalham em conjunto para formar o sistema funcional.

### 1. Bootloader (`boot.s`)

O ponto de entrada do sistema. Escrito em Assembly, este arquivo é responsável por:
* Atender à especificação Multiboot, permitindo que o GRUB o reconheça e carregue.
* Configurar a pilha (`stack`) para o ambiente em C.
* Fazer a transição do Assembly para o C, chamando a função `kmain`, que é o ponto de entrada principal do nosso kernel.

### 2. Kernel (`kernel.c`)

Escrito em C, este é o coração do sistema. Ele contém:
* A função `kmain`, que inicializa os drivers e inicia o loop principal do jogo.
* Os drivers de baixo nível para interação com o hardware.
* A lógica completa da aplicação (Jogo da Forca).

### 3. Drivers de Baixo Nível

Para se comunicar com o hardware, foram implementados drivers simples:

* **Driver de Vídeo (Modo Texto):** A exibição de caracteres na tela é feita escrevendo diretamente na memória de vídeo mapeada no endereço `0xB8000`. Cada caractere na tela é representado por 2 bytes: 1 para o código ASCII e 1 para os atributos de cor. As funções `putc` e `print` abstraem essa complexidade.

* **Driver de Teclado (Polling):** A leitura de teclas foi implementada via **Polling**. O kernel verifica continuamente a porta de status do controlador de teclado (`0x64`) em um loop. Quando um dado está disponível, o *scancode* da tecla é lido da porta de dados (`0x60`) e mapeado para um caractere ASCII. Este método foi escolhido por sua simplicidade e adequação a uma aplicação de loop único como um jogo.

### 4. Lógica do Jogo

A aplicação principal é um Jogo da Forca com as seguintes funcionalidades:
* Uma palavra secreta pré-definida ("KERNEL").
* Exibição da palavra com underscores (`_`).
* Contagem de erros (máximo de 6).
* Desenho visual do enforcado a cada erro.
* Validação de letras já tentadas.
* Condições de vitória e derrota.

## Como Compilar e Executar

O ambiente recomendado para a compilação é um sistema Linux (como Ubuntu/Debian) com as ferramentas necessárias instaladas.

### Pré-requisitos

Certifique-se de ter todos os pacotes necessários. Em um sistema Debian-based, instale-os com:
```bash
sudo apt-get install build-essential nasm qemu-system-x86 grub-pc-bin xorriso mtools
```

### Passo a Passo para Compilação e Execução

1.  **Montar o código Assembly do Bootloader:**
    ```bash
    nasm -f elf32 boot.s -o boot.o
    ```

2.  **Compilar o código C do Kernel:**
    ```bash
    gcc -m32 -ffreestanding -nostdlib -c kernel.c -o kernel.o -lgcc
    ```

3.  **Lincar os arquivos objeto para criar o binário do Kernel:**
    ```bash
    ld -m elf_i386 -T linker.ld -o iso/boot/kernel.bin boot.o kernel.o
    ```

4.  **Criar a Imagem ISO Bootável com o GRUB:**
    Este comando cria a ISO `jogo.iso`, garantindo que ela seja compatível com a arquitetura de PC BIOS de 32 bits.
    ```bash
    grub-mkrescue -d /usr/lib/grub/i386-pc -o jogo.iso iso/
    ```

5.  **Executar o Kernel no QEMU:**
    Este comando inicia o emulador, carrega a ISO como um CD-ROM e inicia o sistema.
    ```bash
    qemu-system-i386 -cdrom jogo.iso
    ```
    
Após a execução, uma janela do QEMU se abrirá e o Jogo da Forca será iniciado.

## Estrutura do Repositório

```
.
├── boot.s          # Código do bootloader em Assembly
├── kernel.c        # Lógica principal do kernel e do jogo em C
├── linker.ld       # Script do linker para posicionar o código em memória
└── iso/
    └── boot/
        ├── kernel.bin  # Binário final (gerado pelo linker)
        └── grub/
            └── grub.cfg    # Arquivo de configuração do GRUB
```

### Vídeo Explicativo

[Vídeo](https://drive.google.com/drive/folders/18ymObB7-tpWtJleKrv3zLws9moR35ceg?usp=drive_link)
