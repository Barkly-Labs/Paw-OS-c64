
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BORDER (*(unsigned char*)0xD020)
#define BACKGROUND (*(unsigned char*)0xD021)

#define SCREEN_WIDTH 40

void rainbow(void)
{
    unsigned char colors[] = {
        COLOR_RED, COLOR_ORANGE,
        COLOR_YELLOW, COLOR_GREEN,
        COLOR_CYAN, COLOR_BLUE,
        COLOR_PURPLE, COLOR_LIGHTRED
    };

    unsigned char i;

    gotoxy(12, 3);

    for (i = 0; i < 8; i++) {
        textcolor(colors[i]);
        cputs("█");
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

void clear_output(void)
{
    unsigned char row;

    textcolor(COLOR_WHITE);

    for (row = 21; row <= 23; row++) {
        gotoxy(2, row);
        cputs("                                      ");
    }
}

void show_help(void)
{
    textcolor(COLOR_CYAN);

    gotoxy(2, 21);
    cputs("AVAILABLE COMMANDS");

    gotoxy(2, 22);
    cputs("HELP  MEM  ABOUT  CLS");
}

void show_about(void)
{

    clrscr();
    textcolor(COLOR_WHITE);

    gotoxy(2, 21);
    cputs("PAW OS - BARKLY LABS");

    gotoxy(2, 22);
    cputs("COMMODORE 64 / 6510");

    gotoxy(2, 23);
    cputs("PEOPLE ARE WORTH DESIGNING FOR");
}

void draw_screen(void)
{
    clrscr();

    BORDER = COLOR_PURPLE;
    BACKGROUND = COLOR_BLACK;

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

    show_memory();

    gotoxy(2, 18);
    cputs("------------------------------------");

    textcolor(COLOR_LIGHTRED);
    gotoxy(2, 19);
    cputs("PAW OS COMMAND INTERPRETER");

    textcolor(COLOR_WHITE);
    gotoxy(2, 20);
    cputs("TYPE HELP FOR COMMANDS");

    gotoxy(2, 24);
    cputs("PAW>");
}

void execute_command(char *command)
{
    clear_output();

    if (strcmp(command, "HELP") == 0) {
        show_help();
    }
    else if (strcmp(command, "MEM") == 0) {
        show_memory();
        textcolor(COLOR_CYAN);
        gotoxy(2, 21);
        cputs("MEMORY STATUS UPDATED");
    }
    else if (strcmp(command, "ABOUT") == 0) {
        show_about();
    }
    else if (strcmp(command, "CLS") == 0) {
        draw_screen();
    }
    else if (command[0] != '\0') {
        textcolor(COLOR_LIGHTRED);
        gotoxy(2, 21);
        cputs("UNKNOWN COMMAND");
    }
}

int main(void)
{
    char command[20];
    unsigned char length;
    unsigned char ch;

    draw_screen();

    while (1) {

        gotoxy(2, 24);
        cputs("PAW>                  ");
        gotoxy(6, 24);

        length = 0;
        command[0] = '\0';

        while (1) {
            ch = cgetc();

            if (ch == 13) {
                command[length] = '\0';
                break;
            }

            if (ch == 20 && length > 0) {
                length--;
                command[length] = '\0';
                cputc(20);
            }
            else if (ch >= 32 && ch <= 126 &&
                     length < sizeof(command) - 1) {

                ch = toupper(ch);
                command[length++] = ch;
                cputc(ch);
            }
        }

        execute_command(command);
    }

    return 0;
}
