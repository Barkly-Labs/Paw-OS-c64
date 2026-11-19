
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BORDER (*(unsigned char*)0xD020)
#define BACKGROUND (*(unsigned char*)0xD021)

void rainbow(void)
{
    unsigned char colors[] = {
        COLOR_RED,
        COLOR_ORANGE,
        COLOR_YELLOW,
        COLOR_GREEN,
        COLOR_CYAN,
        COLOR_BLUE,
        COLOR_PURPLE,
        COLOR_LIGHTRED
    };

    unsigned char i;

    gotoxy(12, 3);

    for (i = 0; i < 8; i++) {
        textcolor(colors[i]);
        cputs("[]");
    }
}

void paw_logo(void)
{
    textcolor(COLOR_LIGHTRED);

    gotoxy(1, 1);
    cputs(" o o ");

    gotoxy(1, 2);
    cputs("o   o");

    gotoxy(1, 3);
    cputs(" \\_/ ");
}

void show_memory(void)
{
    unsigned int total;
    unsigned int largest;

    total = _heapmemavail();
    largest = _heapmaxavail();

    textcolor(COLOR_WHITE);

    gotoxy(2, 13);
    cprintf("TOTAL PHYSICAL RAM: 64 KB   ");

    gotoxy(2, 14);
    cprintf("FREE HEAP: %u BYTES   ", total);

    gotoxy(2, 15);
    cprintf("LARGEST BLOCK: %u BYTES   ", largest);
}

void show_help(void)
{
    textcolor(COLOR_CYAN);

    cputs("\nAVAILABLE COMMANDS\n");
    cputs("------------------\n");
    cputs("HELP  - COMMAND LIST\n");
    cputs("MEM   - MEMORY STATUS\n");
    cputs("CLS   - CLEAR SCREEN\n");
    cputs("ABOUT - SYSTEM INFO\n");
}

void show_about(void)
{
    cputs("\nPAW OS - BARKLY LABS\n");
    cputs("C64 EDITION\n");
    cputs("PEOPLE ARE WORTH DESIGNING FOR\n");
    cputs("TARGET: COMMODORE 64 / 6510\n");
}

void draw_screen(void)
{
    clrscr();

    BORDER = COLOR_PURPLE;
    BACKGROUND = COLOR_BLUE;

    paw_logo();
    rainbow();

    textcolor(COLOR_LIGHTRED);
    gotoxy(10, 6);
    cputs("PAW OS");

    textcolor(COLOR_WHITE);
    gotoxy(5, 8);
    cputs("BARKLY LABS C64 EDITION");

    gotoxy(2, 11);
    cputs("------------------------------------");

    gotoxy(2, 18);
    cputs("------------------------------------");

    textcolor(COLOR_LIGHTRED);
    gotoxy(2, 20);
    cputs("PAW OS COMMAND INTERPRETER");

    textcolor(COLOR_WHITE);
    gotoxy(2, 22);
    cputs("TYPE HELP FOR COMMANDS");

    gotoxy(2, 24);
    cputs("PAW>");
}

int main(void)
{
    char command[20];
    unsigned char length;
    unsigned char ch;

    draw_screen();

    show_memory();

    while (1) {
        gotoxy(6, 24);
        cputs("                    ");
        gotoxy(6, 24);

        length = 0;

        while (1) {
            ch = cgetc();

            if (ch == 13) {
                command[length] = '\0';
                break;
            }

            if (ch == 20 && length > 0) {
                length--;
                cputc(20);
                continue;
            }

            if (ch >= 32 && ch <= 126 &&
                length < sizeof(command) - 1) {
                ch = toupper(ch);
                command[length++] = ch;
                cputc(ch);
            }
        }

        if (strcmp(command, "HELP") == 0) {
            show_help();
        }
        else if (strcmp(command, "MEM") == 0) {
            show_memory();
        }
        else if (strcmp(command, "ABOUT") == 0) {
            show_about();
        }
        else if (strcmp(command, "CLS") == 0) {
            draw_screen();
            show_memory();
        }
        else if (command[0] != '\0') {
            textcolor(COLOR_LIGHTRED);
            cputs("\nUNKNOWN COMMAND\n");
        }
    }

    return 0;
}
