#include <conio.h>
#include <stdio.h>
#include <cbm.h>
#include "paw_hw.h"
#include "paw.h"

static const unsigned char rainbow_colors[8] = {
    COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN,
    COLOR_CYAN, COLOR_BLUE, COLOR_PURPLE, COLOR_LIGHTRED
};

static const unsigned char rainbow_widths[8] = {
    20, 18, 16, 14, 12, 10, 8, 6
};

#define RAINBOW_X 2
#define RAINBOW_Y 2

static void draw_rainbow_frame(unsigned char visible_width)
{
    unsigned char i;
    unsigned char j;
    unsigned char width;

    for (i = 0; i < 8; ++i) {
        width = visible_width;
        if (width > rainbow_widths[i]) width = rainbow_widths[i];

        gotoxy(RAINBOW_X, (unsigned char)(RAINBOW_Y + i));
        textcolor(rainbow_colors[i]);
        revers(1);
        for (j = 0; j < width; ++j) cputc(' ');
        revers(0);
    }
}

static void animate_rainbow(void)
{
    unsigned char width;

    for (width = 1; width <= rainbow_widths[0]; ++width) {
        draw_rainbow_frame(width);
        waitvsync();
    }
    revers(0);
    textcolor(COLOR_WHITE);
}

void rainbow(void)
{
    unsigned char i;
    unsigned char j;

    for (i = 0; i < 8; i++) {
        gotoxy(RAINBOW_X, (unsigned char)(RAINBOW_Y + i));
        textcolor(rainbow_colors[i]);
        revers(1);
        for (j = 0; j < rainbow_widths[i]; ++j) cputc(' ');
        revers(0);
    }

    /* Leave ordinary text mode and a predictable foreground color. */
    revers(0);
    textcolor(COLOR_WHITE);
}

void paw_logo(void)
{
    textcolor(COLOR_LIGHTRED);

    /* Four rounded toe pads in the same position beside the boot rainbow. */
    gotoxy(22, 2);
    cputs("(O)   (O)");

    gotoxy(22, 3);
    cputs("  (O) (O)");

    /* The larger palm pad uses simple PETSCII-compatible ASCII characters. */
    textcolor(COLOR_YELLOW);
    gotoxy(22, 4);
    cputs("   .----.");

    gotoxy(22, 5);
    cputs("  / PAW  \\");

    gotoxy(22, 6);
    cputs("  \\______/");
}

void clear_output(void)
{
    unsigned char row;

    textcolor(COLOR_WHITE);

    for (row = 21; row <= 23; row++) {
        gotoxy(2, row);
        cputs("                                     ");
    }
}

void show_help(void)
{
    clrscr();
    textcolor(COLOR_CYAN);

    gotoxy(2, 21);
    cputs("COMMANDS");

    gotoxy(2, 22);
    cputs("HELP MEM ABOUT CLS DISK DIR FILES PWD");
    gotoxy(2, 23);
    cputs("CD TYPE DELETE RENAME PHONE DTMF DIAL");
    gotoxy(2, 24);
    cputs("DTMFTEST MODEM SYNTH");
}

void show_about(void)
{
    clrscr();

    BORDER = COLOR_PURPLE;
    BACKGROUND = COLOR_BLACK;

    /* Reuse the same paw and rainbow renderers as the PAW OS boot screen. */
    paw_logo();
    rainbow();

    /* Keep the About copy below the shared eight-row brand header. */
    textcolor(COLOR_LIGHTRED);
    gotoxy(16, 11);
    cputs("PAW OS");

    textcolor(COLOR_CYAN);
    gotoxy(17, 12);
    cputs("ABOUT");

    textcolor(COLOR_WHITE);
    gotoxy(5, 14);
    cputs("A SMART TERMINAL FOR THE C64");

    gotoxy(8, 15);
    cputs("BUILT FOR COMMUNICATION");

    textcolor(COLOR_LIGHTRED);
    gotoxy(15, 17);
    cputs("CREATED BY");

    textcolor(COLOR_YELLOW);
    gotoxy(14, 18);
    cputs("BARKLY LABS");

    textcolor(COLOR_WHITE);
    gotoxy(12, 19);
    cputs("PIPER MAE BARKLY");

    textcolor(COLOR_CYAN);
    gotoxy(11, 22);
    cputs("COMMODORE 64 / 6510");

    /* Preserve the existing About-screen theme playback and return path. */
    play_ode_to_joy();
}

static void draw_home_screen(unsigned char animate)
{
    clrscr();

    BORDER = COLOR_PURPLE;
    BACKGROUND = COLOR_BLUE;

    paw_logo();
    textcolor(COLOR_CYAN);
    gotoxy(2, 12);
    cputs("PAW OS STARTING...");
    if (animate) animate_rainbow();
    else rainbow();

    textcolor(COLOR_LIGHTRED);
    gotoxy(16, 10);
    cputs("PAW OS");

    textcolor(COLOR_WHITE);
    gotoxy(9, 11);
    cputs("BARKLY LABS C64 EDITION");

    /* The subtitle is on row 11; startup status begins below it. */
    gotoxy(2, 12);
    cputs("PAW OS SYSTEM STATUS                  ");

    show_memory();

    textcolor(COLOR_LIGHTGREEN);
    gotoxy(2, 17); cputs("TERMINAL: READY");
    gotoxy(2, 18); cputs("SID SOUND: READY");
    textcolor(COLOR_CYAN);
    gotoxy(2, 19); cputs("IEC DEVICE: 8 (ON DEMAND)");

    textcolor(COLOR_LIGHTGREEN);
    gotoxy(2, 21); cputs("READY.");
    textcolor(COLOR_WHITE);
    gotoxy(2, 22); cputs("TYPE HELP FOR COMMANDS");

    gotoxy(2, 24);
    cputs("PAW>");
}

void boot_screen(void)
{
    draw_home_screen(1);
}

void draw_screen(void)
{
    draw_home_screen(0);
}
