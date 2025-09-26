// kernel.c

// --- Funções de I/O de Baixo Nível ---

volatile unsigned short *video_memory = (unsigned short *)0xB8000;
int cursor_x = 0;
int cursor_y = 0;

// Limpa a tela
void clear_screen() {
    for (int y = 0; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            const int index = y * 80 + x;
            video_memory[index] = (unsigned short)' ' | (unsigned short)0x0F << 8;
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}

// Coloca um caractere na tela na posição atual do cursor
void putc(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        const int index = cursor_y * 80 + cursor_x;
        video_memory[index] = (unsigned short)c | (unsigned short)0x0F << 8; // Branco no preto
        cursor_x++;
    }

    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= 25) {
        clear_screen();
    }
}

// Imprime uma string na tela
void print(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        putc(str[i++]);
    }
}

// Funções de porta de I/O (Assembly inline)
static inline unsigned char inb(unsigned short port) {
    unsigned char data;
    asm volatile("inb %1, %0" : "=a"(data) : "d"(port));
    return data;
}

// Mapa de scancodes do teclado para caracteres (layout US simplificado)
const char scancode_map[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' '
};

// Lê um caractere do teclado (bloqueante)
char getc() {
    unsigned char scancode;
    while(1) {
        // Espera a flag do teclado indicar que há dados
        if (inb(0x64) & 1) {
            scancode = inb(0x60);
            // Verifica se é uma tecla de "pressionar" (scancode < 0x80)
            if (scancode < 0x80 && scancode_map[scancode]) {
                char c = scancode_map[scancode];
                putc(c); // Echo do caractere na tela
                if (c == '\n') return c; // Deixa o loop principal lidar com o enter
                return c;
            }
        }
    }
}

// --- Funções da Biblioteca C Reimplementadas ---

int strlen(const char *str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char toupper(char c) {
    if (c >= 'a' && c <= 'z') return c - 32;
    return c;
}

// Converte int para string (necessário para imprimir números)
void itoa(int n, char* str) {
    int i = 0;
    if (n == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    while(n > 0) {
        str[i++] = (n % 10) + '0';
        n = n / 10;
    }
    str[i] = '\0';
    // Inverte a string
    int start = 0, end = i - 1;
    while(start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// --- Lógica Principal do Jogo ---

void desenhar_forca(int erros) {
    print("  +---+\n");
    print("  |   |\n");
    print("  |   "); putc(erros >= 1 ? 'O' : ' '); print("\n");
    print("  |  "); putc(erros >= 3 ? '/' : ' '); putc(erros >= 2 ? '|' : ' '); putc(erros >= 4 ? '\\' : ' '); print("\n");
    print("  |  "); putc(erros >= 5 ? '/' : ' '); print(" "); putc(erros >= 6 ? '\\' : ' '); print("\n");
    print("  |\n");
    print("=========\n");
}

void kmain(void) {
    clear_screen();

    const char *palavraSecreta = "KERNEL";
    int tamanhoPalavra = strlen(palavraSecreta);
    
    char letrasDescobertas[tamanhoPalavra + 1];
    char letrasTentadas[27] = {0}; // +1 para o terminador nulo
    int numLetrasTentadas = 0;
    
    int erros = 0;

    for (int i = 0; i < tamanhoPalavra; i++) {
        letrasDescobertas[i] = '_';
    }
    letrasDescobertas[tamanhoPalavra] = '\0';
    
    print("Bem-vindo ao Jogo da Forca no Kernel Simples!\n\n");

    while (erros < 6) {
        desenhar_forca(erros);
        print("Palavra: ");
        print(letrasDescobertas);
        print("\n");
        
        print("Letras tentadas: ");
        print(letrasTentadas);
        print("\n");
        
        print("Digite uma letra e pressione Enter: ");
        char tentativa = 0;
        
        // Pega a letra
        while(tentativa == 0 || tentativa == '\n') {
           tentativa = getc();
        }
        // Pega o enter
        while(getc() != '\n');
        
        tentativa = toupper(tentativa);

        int jaTentou = 0;
        for (int i = 0; i < numLetrasTentadas; i++) {
            if (letrasTentadas[i] == tentativa) {
                jaTentou = 1;
                break;
            }
        }

        if (jaTentou) {
            print("\nVoce ja tentou essa letra!\n\n");
            continue;
        }

        letrasTentadas[numLetrasTentadas++] = tentativa;
        letrasTentadas[numLetrasTentadas] = ' ';

        int letraEncontrada = 0;
        for (int i = 0; i < tamanhoPalavra; i++) {
            if (palavraSecreta[i] == tentativa) {
                letrasDescobertas[i] = tentativa;
                letraEncontrada = 1;
            }
        }

        if (letraEncontrada) {
            print("\nBoa! A letra esta na palavra.\n\n");
        } else {
            print("\nQue pena! A letra nao esta na palavra.\n\n");
            erros++;
        }

        if (strcmp(palavraSecreta, letrasDescobertas) == 0) {
            clear_screen();
            print("===================================\n");
            print(" Parabens! Voce venceu!\n");
            print(" A palavra era: ");
            print(palavraSecreta);
            print("\n");
            print("===================================\n");
            return;
        }
    }

    clear_screen();
    desenhar_forca(erros);
    print("\n\nGAME OVER! Voce perdeu!\n");
    print("A palavra era: ");
    print(palavraSecreta);
    print("\n");
}