#include <conio.h>
#include <stdio.h>
#include <cbm.h>
#include "paw_hw.h"
#include "paw.h"
#include "paw_sid.h"

/*
 * PAW OS puppy paw sprite
 *
 * Sprite data: $0340-$037F
 * Sprite pointer: $07F8
 * Sprite number: 0
 *
 * Uses the cassette buffer. Do not use cassette routines
 * while this sprite allocation is active.
 */

#define VIC_SPR0_X       (*(volatile unsigned char*)0xD000)
#define VIC_SPR0_Y       (*(volatile unsigned char*)0xD001)
#define VIC_SPR_X_MSB    (*(volatile unsigned char*)0xD010)
#define VIC_SPR_ENABLE   (*(volatile unsigned char*)0xD015)
#define VIC_SPR_Y_EXP    (*(volatile unsigned char*)0xD017)
#define VIC_SPR_PRIORITY (*(volatile unsigned char*)0xD01B)
#define VIC_SPR_MULTICOL (*(volatile unsigned char*)0xD01C)
#define VIC_SPR_X_EXP    (*(volatile unsigned char*)0xD01D)

#define VIC_SPR_MC0      (*(volatile unsigned char*)0xD025)
#define VIC_SPR_MC1      (*(volatile unsigned char*)0xD026)
#define VIC_SPR0_COLOR   (*(volatile unsigned char*)0xD027)

#define VIC_SPR0_POINTER (*(volatile unsigned char*)0x07F8)

#define PAW_SPRITE_RAM ((volatile unsigned char*)0x0340)

static const unsigned char paw_sprite_data[64] = {
    /* Four pink toe beans with purple edging */
    0x03, 0xF3, 0xF0,
    0x03, 0x7F, 0x70,
    0xFF, 0x5D, 0x7F,
    0xDF, 0x5D, 0x7D,
    0xD7, 0x7F, 0x75,
    0xD7, 0xF3, 0xF5,
    0xDF, 0x00, 0x3D,
    0xFC, 0x00, 0x0F,

    /* Golden central pad */
    0x0F, 0xFF, 0xF0,
    0x3E, 0xAA, 0xBC,
    0x3A, 0xAA, 0xAC,
    0x3A, 0xAA, 0xAC,
    0x3A, 0xAA, 0xAC,
    0x3A, 0xAA, 0xAC,
    0x3A, 0xAA, 0xAC,
    0x3A, 0xAA, 0xAC,
    0x3A, 0xAA, 0xAC,
    0x3E, 0xAA, 0xBC,
    0x0E, 0xAA, 0xB0,
    0x0F, 0xAA, 0xF0,
    0x03, 0xEB, 0xC0,

    /* Required 64th byte */
    0x00
};

void paw_sprite_show(void)
{
    unsigned char i;

    /* Copy sprite graphics into VIC-II accessible RAM */
    for (i = 0; i < 64; ++i) {
        PAW_SPRITE_RAM[i] = paw_sprite_data[i];
    }

    /* Sprite 0 points to $0340 / 64 = 13 */
    VIC_SPR0_POINTER = 13;

    /* Multicolor: pink, purple outline, golden pad */
    VIC_SPR_MC0 = 10;
    VIC_SPR_MC1 = 4;
    VIC_SPR0_COLOR = 7;

    /* Position near the upper-right corner */
    VIC_SPR0_X = 250;
    VIC_SPR0_Y = 70;

    /* Keep X position below 256 */
    VIC_SPR_X_MSB &= 0xFE;

    /* Double the sprite's width and height */
    VIC_SPR_X_EXP |= 0x01;
    VIC_SPR_Y_EXP |= 0x01;

    /* Enable multicolor mode */
    VIC_SPR_MULTICOL |= 0x01;

    /* Draw in front of text */
    VIC_SPR_PRIORITY &= 0xFE;

    /* Enable sprite 0 */
    VIC_SPR_ENABLE |= 0x01;
}

void paw_sprite_hide(unsigned char sprite)
{
    VIC_SPR_ENABLE &= ~(1 << sprite);
}

void paw_sprites_clear(void)
{
    VIC_SPR_ENABLE = 0;
}

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

    rainbow();
    paw_sprite_show();

    textcolor(COLOR_LIGHTRED);
    gotoxy(16, 11);
    cputs("PAW OS");

    textcolor(COLOR_WHITE);
    gotoxy(2, 12);
    cputs("COMMANDS");

    textcolor(COLOR_CYAN);
    gotoxy(2, 14);
    cputs("HELP MEM ABOUT CLS DISK DIR FILES PWD");
    gotoxy(2, 15);
    cputs("CD TYPE DELETE RENAME PHONE DTMF DIAL");
    gotoxy(2, 16);
    cputs("DTMFTEST MODEM SYNTH RADIO");
}

void show_about(void)
{
    clrscr();

    BORDER = COLOR_PURPLE;
    BACKGROUND = COLOR_BLACK;

    /* Reuse the same paw and rainbow renderers as the PAW OS boot screen. */

    rainbow();
    paw_sprite_show();

    /* Keep the About copy below the shared eight-row brand header. */
    textcolor(COLOR_LIGHTRED);
    gotoxy(16, 11);
    cputs("PAW OS");

    textcolor(COLOR_CYAN);
    gotoxy(17, 12);
    cputs("ABOUT");

    textcolor(COLOR_WHITE);
    gotoxy(5, 14);
    cputs("A SMART Interface FOR THE C64");

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
    BACKGROUND = COLOR_BLACK;

    paw_sprite_show();
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

    textcolor(COLOR_WHITE);
    gotoxy(2, 22); cputs("TYPE HELP FOR COMMANDS");

    gotoxy(2, 24);
    cputs("PAW>");
    play_midi_sid();

}

/* PAW RADIO FULL-SCREEN MODULE */

 void draw_radio_screen(void)
{
    clrscr();
    paw_sprites_clear();
    
    BORDER = COLOR_PURPLE;
    BACKGROUND = COLOR_BLACK;


    /* Header */
    textcolor(COLOR_LIGHTRED);
    gotoxy(12, 2);
    cputs("PAW RADIO");

    textcolor(COLOR_WHITE);
    gotoxy(7, 3);
    cputs("BARKLY LABS - C64 RADIO STATION");

    /* Divider */
    textcolor(COLOR_CYAN);
    gotoxy(1, 5);
    cputs("----------------------------------------");

    /* Menu */
    textcolor(COLOR_LIGHTGREEN);
    gotoxy(4, 7);
    cputs("RADIO OPERATIONS");

    textcolor(COLOR_WHITE);
    gotoxy(5, 9);  cputs("1. RADIO CONTROL");
    gotoxy(5, 10); cputs("2. BAND EXPLORER");
    gotoxy(5, 11); cputs("3. SSTV STUDIO");
    gotoxy(5, 12); cputs("4. PACKET TERMINAL");
    gotoxy(5, 13); cputs("5. MORSE LAB");
    gotoxy(5, 14); cputs("6. STATION LOG");
    gotoxy(5, 15); cputs("7. HARDWARE STATUS");

    /* Station status panel */
    textcolor(COLOR_CYAN);
    gotoxy(25, 7);
    cputs("STATION STATUS");

    textcolor(COLOR_WHITE);
    gotoxy(25, 9);  cputs("MODE: STANDBY");
    gotoxy(25, 10); cputs("RX: READY");
    gotoxy(25, 11); cputs("TX: DISABLED");
    gotoxy(25, 12); cputs("SID: READY");
    gotoxy(25, 13); cputs("LINK: LOCAL");

    /* Footer */
    textcolor(COLOR_LIGHTGREEN);
    gotoxy(1, 18);
    cputs("----------------------------------------");

    textcolor(COLOR_WHITE);
    gotoxy(3, 20);
    cputs("SELECT A RADIO MODULE USING THE KEYS");

    textcolor(COLOR_LIGHTRED);
    gotoxy(3, 22);
    cputs("Q / ESC: RETURN TO PAW OS");

    textcolor(COLOR_CYAN);
    gotoxy(3, 24);
    cputs("PAW RADIO :: SYSTEM READY");
}

void boot_screen(void)
{
    draw_home_screen(1);
}

void draw_screen(void)
{
    draw_home_screen(0);
}
