
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BORDER (*(unsigned char*)0xD020)
#define BACKGROUND (*(unsigned char*)0xD021)

#define SCREEN_WIDTH 40

/* SID voice 1 registers */
#define SID_FREQ_LO  (*(volatile unsigned char*)0xD400)
#define SID_FREQ_HI  (*(volatile unsigned char*)0xD401)
#define SID_CONTROL  (*(volatile unsigned char*)0xD404)
#define SID_ATTACK   (*(volatile unsigned char*)0xD405)
#define SID_SUSTAIN  (*(volatile unsigned char*)0xD406)
#define SID_VOLUME   (*(volatile unsigned char*)0xD418)

/* NTSC SID frequency values */
#define NOTE_C5  8584
#define NOTE_D5  9634
#define NOTE_E5  10814
#define NOTE_E4  5407
#define NOTE_F4  5728
#define NOTE_G4  6430

void play_ode_to_joy(void)
{
    static const unsigned int melody[] = {
        NOTE_E4, NOTE_E4, NOTE_F4, NOTE_G4,
        NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D5 / 2,
        NOTE_C5 / 2, NOTE_C5 / 2, NOTE_D5 / 2,
        NOTE_E4, NOTE_E4, NOTE_D5 / 2, NOTE_D5 / 2,
        NOTE_E4
    };

    unsigned char i;
    unsigned char repeat;
    unsigned char frame;

    /* Configure SID voice 1 */
    SID_CONTROL = 0;
    SID_ATTACK = 0x02;
    SID_SUSTAIN = 0xF8;
    SID_VOLUME = 15;

    for (repeat = 0; repeat < 4; repeat++) {

        for (i = 0; i < 16; i++) {

            SID_FREQ_LO = melody[i] & 0xFF;
            SID_FREQ_HI = melody[i] >> 8;

            SID_CONTROL = 0x11; /* Triangle wave + gate */

            /* Approximately 28 frames per note */
            for (frame = 0; frame < 28; frame++) {
                waitvsync();
            }
        }
    }

    /* Turn off the SID voice */
    SID_CONTROL = 0x10;

    for (frame = 0; frame < 30; frame++) {
        waitvsync();
    }

    SID_VOLUME = 0;
}

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
        cputc(18);   // Reverse video ON
        cputc(' ');  // Solid colored square
        cputc(146);  // Reverse video OFF
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
    cputs("A SMART TERMINAL FOR THE C64");

    gotoxy(5, 13);
    cputs("BUILT FOR COMMUNICATION");

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
