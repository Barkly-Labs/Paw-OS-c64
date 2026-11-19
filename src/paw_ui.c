#include <conio.h>
#include <stdio.h>
#include "paw_hw.h"
#include "paw.h"

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
        revers(1);
        cputc(' ');
        revers(0);
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
    cputs("COMMANDS");

    gotoxy(2, 22);
<<<<<<< HEAD
    cputs("HELP MEM ABOUT CLS DISK DIR FILES PWD");
    gotoxy(2, 23);
    cputs("CD TYPE DELETE RENAME PHONE DTMF DIAL");
    gotoxy(2, 24);
    cputs("DTMFTEST MODEM SYNTH");
=======
    cputs("HELP MEM ABOUT CLS PHONE DIAL MODEM");
    gotoxy(2, 23);
    cputs("DTMF number [FAST] DTMFTEST SYNTH");
>>>>>>> b5800ffa93ad7f862755c01629bb8cbec9eae056
}

void show_about(void)
{
    clrscr();

    BORDER = COLOR_PURPLE;
    BACKGROUND = COLOR_BLACK;

    /* PAW logo and rainbow header */
    paw_logo();
    rainbow();

    /* Main title */
    textcolor(COLOR_LIGHTRED);
    gotoxy(14, 6);
    cputs("PAW OS");

    textcolor(COLOR_CYAN);
    gotoxy(13, 8);
    cputs("ABOUT");

    /* Description */
    textcolor(COLOR_WHITE);
    gotoxy(3, 11);
    cputs("A Smol OS for the c 64 / 6510");

    gotoxy(5, 13);
    cputs("BUILT WITH BARKLY LABS C64 SDK");

    /* Creator credits */
    textcolor(COLOR_LIGHTRED);
    gotoxy(13, 16);
    cputs("CREATED BY");

    textcolor(COLOR_YELLOW);
    gotoxy(10, 18);
    cputs("BARKLY LABS");

    textcolor(COLOR_WHITE);
    gotoxy(8, 20);
    cputs("PIPER MAE BARKLY");

    /* Hardware information */
    textcolor(COLOR_CYAN);
    gotoxy(8, 23);
    cputs("COMMODORE 64 / 6510");

    /* Play the PAW OS theme */
    play_ode_to_joy();
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
