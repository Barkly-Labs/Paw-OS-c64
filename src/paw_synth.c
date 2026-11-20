#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <cbm.h>
#include "paw_hw.h"
#include "paw_synth.h"

/* PAW Synth uses SID voice 3, leaving the PHONE/DTMF voices 1 and 2 alone. */
#define SID3_FREQ_LO (*(volatile unsigned char*)0xD40E)
#define SID3_FREQ_HI (*(volatile unsigned char*)0xD40F)
#define SID3_PW_LO   (*(volatile unsigned char*)0xD410)
#define SID3_PW_HI   (*(volatile unsigned char*)0xD411)
#define SID3_CONTROL (*(volatile unsigned char*)0xD412)
#define SID3_ATTACK  (*(volatile unsigned char*)0xD413)
#define SID3_SUSTAIN (*(volatile unsigned char*)0xD414)
#define SID_FILTER_LO (*(volatile unsigned char*)0xD415)
#define SID_FILTER_HI (*(volatile unsigned char*)0xD416)
#define SID_FILTER   (*(volatile unsigned char*)0xD417)
#define SID_VOLUME_REG (*(volatile unsigned char*)0xD418)
#define JOY2_PORT (*(volatile unsigned char*)0xDC00)

#define WAVE_TRIANGLE 0
#define WAVE_SAW      1
#define WAVE_PULSE    2
#define WAVE_NOISE    3

#define SET_WAVE   0
#define SET_OCTAVE 1
#define SET_PULSE  2
#define SET_ATTACK 3
#define SET_DECAY  4
#define SET_SUSTAIN 5
#define SET_RELEASE 6
#define SET_VOLUME 7
#define SET_FILTER 8
#define SET_COUNT  9

/* NTSC tuning words, chromatic C through B, octaves 2 through 6. */
static const unsigned int note_words[5][12] = {
    {1073,1137,1204,1276,1352,1432,1517,1608,1703,1804,1912,2025},
    {2146,2274,2409,2552,2704,2864,3035,3215,3406,3609,3824,4051},
    {4292,4547,4817,5104,5407,5729,6070,6430,6813,7218,7647,8102},
    {8584,9094,9635,10208,10815,11458,12139,12861,13626,14436,15294,16204},
    {17167,18188,19270,20415,21629,22916,24278,25722,27251,28872,30589,32407}
};

static const char *note_names[12] = {
    "C", "C#", "D", "D#", "E", "F",
    "F#", "G", "G#", "A", "A#", "B"
};

static const char *wave_names[4] = {
    "TRIANGLE", "SAWTOOTH", "PULSE", "NOISE"
};

static unsigned char wave = WAVE_TRIANGLE;
static unsigned char octave = 4;
static unsigned char pulse_width = 128;
static unsigned char attack = 2;
static unsigned char decay = 8;
static unsigned char sustain = 12;
static unsigned char release = 8;
static unsigned char volume = 12;
static unsigned char filter_enabled = 0;
static unsigned char selected_setting = SET_WAVE;
static unsigned char current_note = 0;
static unsigned char note_active = 0;
static unsigned char saved_sid[11];

static void sid_save(void)
{
    saved_sid[0] = SID3_FREQ_LO;
    saved_sid[1] = SID3_FREQ_HI;
    saved_sid[2] = SID3_PW_LO;
    saved_sid[3] = SID3_PW_HI;
    saved_sid[4] = SID3_CONTROL;
    saved_sid[5] = SID3_ATTACK;
    saved_sid[6] = SID3_SUSTAIN;
    saved_sid[7] = SID_VOLUME_REG;
    saved_sid[8] = SID_FILTER_LO;
    saved_sid[9] = SID_FILTER_HI;
    saved_sid[10] = SID_FILTER;
}

static void sid_restore(void)
{
    /* Release the Synth note before restoring the voice and shared volume. */
    SID3_CONTROL = 0;
    SID3_FREQ_LO = saved_sid[0];
    SID3_FREQ_HI = saved_sid[1];
    SID3_PW_LO = saved_sid[2];
    SID3_PW_HI = saved_sid[3];
    SID3_ATTACK = saved_sid[5];
    SID3_SUSTAIN = saved_sid[6];
    SID3_CONTROL = saved_sid[4];
    SID_FILTER_LO = saved_sid[8];
    SID_FILTER_HI = saved_sid[9];
    SID_FILTER = saved_sid[10];
    SID_VOLUME_REG = saved_sid[7];
}

static void draw_setting_value(unsigned char setting)
{
    switch (setting) {
    case SET_WAVE: cputs(wave_names[wave]); break;
    case SET_OCTAVE: cprintf("%u", octave); break;
    case SET_PULSE: cprintf("%u", pulse_width); break;
    case SET_ATTACK: cprintf("%u", attack); break;
    case SET_DECAY: cprintf("%u", decay); break;
    case SET_SUSTAIN: cprintf("%u", sustain); break;
    case SET_RELEASE: cprintf("%u", release); break;
    case SET_VOLUME: cprintf("%u", volume); break;
    case SET_FILTER: cputs(filter_enabled ? "ON" : "OFF"); break;
    }
}

static void draw_line(unsigned char y, const char *label,
                      unsigned char setting)
{
    gotoxy(3, y);
    if (selected_setting == setting) {
        textcolor(COLOR_YELLOW);
        cputc('>');
    } else {
        textcolor(COLOR_WHITE);
        cputc(' ');
    }
    cputs(label);
    gotoxy(24, y);
    draw_setting_value(setting);
    cputs("           ");
}

static void draw_screen(void)
{
    clrscr();
    BORDER = COLOR_PURPLE;
    BACKGROUND = COLOR_BLUE;

    textcolor(COLOR_YELLOW);
    gotoxy(13, 1); cputs("PAW SYNTH");
    textcolor(COLOR_CYAN);
    gotoxy(5, 2); cputs("A FRIENDLY LITTLE SID STUDIO");

    textcolor(COLOR_WHITE);
    gotoxy(3, 4); cputs("SOUND");
    gotoxy(3, 5); cputs("INSTRUMENT:");
    gotoxy(24, 5); draw_setting_value(SET_WAVE);
    gotoxy(3, 6); cputs("WAVEFORM:");
    gotoxy(24, 6); cputs(wave_names[wave]);
    gotoxy(3, 7); cputs("OCTAVE:");
    gotoxy(24, 7); cprintf("%u", octave);
    gotoxy(3, 8); cputs("CURRENT NOTE:");
    gotoxy(24, 8);
    if (note_active) {
        cputs(note_names[current_note]);
        cprintf("%u", octave);
    } else {
        cputs("NONE       ");
    }
    gotoxy(3, 9); cputs("SID VOICE 3:");
    gotoxy(24, 9); cputs(note_active ? "PLAYING" : "READY  ");

    textcolor(COLOR_CYAN);
    gotoxy(3, 11); cputs("SHAPE THE SOUND");
    draw_line(12, "WAVE", SET_WAVE);
    draw_line(13, "OCTAVE", SET_OCTAVE);
    draw_line(14, "PULSE WIDTH", SET_PULSE);
    draw_line(15, "ATTACK", SET_ATTACK);
    draw_line(16, "DECAY", SET_DECAY);
    draw_line(17, "SUSTAIN", SET_SUSTAIN);
    draw_line(18, "RELEASE", SET_RELEASE);
    draw_line(19, "VOLUME", SET_VOLUME);
    draw_line(20, "LOW-PASS FILTER", SET_FILTER);

    textcolor(note_active ? COLOR_LIGHTGREEN : COLOR_CYAN);
    gotoxy(3, 21);
    if (note_active) cputs("***  PAW SOUND IS ON  ***");
    else cputs("   READY WHEN YOU ARE   ");

    textcolor(COLOR_WHITE);
    gotoxy(2, 22); cputs("A W S E D F T G Y H U J = PLAY NOTES");
    gotoxy(2, 23); cputs("Z/X OCTAVE SPACE STOP ESC / EXIT");
    textcolor(COLOR_CYAN);
    gotoxy(2, 24); cputs("ARROWS / JOY 2: SELECT + CHANGE VALUES");
}

static void note_off(void)
{
    SID3_CONTROL = (unsigned char)(1 << 4);
    note_active = 0;
    draw_screen();
}

static void apply_filter(void)
{
    if (filter_enabled) {
        /* A moderate cutoff and low resonance keep the first sound gentle. */
        SID_FILTER_LO = 0;
        SID_FILTER_HI = 0x40;
        SID_FILTER = 0x14;
    } else {
        SID_FILTER = 0;
    }
    SID_VOLUME_REG = (unsigned char)(volume | (filter_enabled ? 0x10 : 0));
}

static void note_on(unsigned char note)
{
    unsigned int word;
    unsigned char control;

    if (note > 11 || octave < 2 || octave > 6) return;
    current_note = note;
    word = note_words[octave - 2][note];

    SID3_CONTROL = 0;
    SID3_FREQ_LO = (unsigned char)(word & 0xFF);
    SID3_FREQ_HI = (unsigned char)(word >> 8);
    SID3_PW_LO = (unsigned char)(pulse_width << 4);
    SID3_PW_HI = (unsigned char)(pulse_width >> 4);
    SID3_ATTACK = (unsigned char)((attack << 4) | (decay & 0x0F));
    SID3_SUSTAIN = (unsigned char)((sustain << 4) | (release & 0x0F));
    apply_filter();

    control = (unsigned char)(wave == WAVE_TRIANGLE ? 0x10 :
              wave == WAVE_SAW ? 0x20 :
              wave == WAVE_PULSE ? 0x40 : 0x80);
    SID3_CONTROL = (unsigned char)(control | 0x01);
    note_active = 1;
    draw_screen();
}

static void select_setting(int delta)
{
    if (delta < 0) {
        if (selected_setting == 0) selected_setting = SET_COUNT - 1;
        else --selected_setting;
    } else {
        selected_setting = (unsigned char)((selected_setting + 1) % SET_COUNT);
    }
    draw_screen();
}

static void adjust_setting(int delta)
{
    switch (selected_setting) {
    case SET_WAVE:
        if (delta > 0) wave = (unsigned char)((wave + 1) & 3);
        else wave = (unsigned char)((wave + 3) & 3);
        break;
    case SET_OCTAVE:
        if (delta > 0 && octave < 6) ++octave;
        else if (delta < 0 && octave > 2) --octave;
        break;
    case SET_PULSE:
        if (delta > 0 && pulse_width <= 224) pulse_width += 16;
        else if (delta < 0 && pulse_width >= 16) pulse_width -= 16;
        break;
    case SET_ATTACK:
        if (delta > 0 && attack < 15) ++attack;
        else if (delta < 0 && attack > 0) --attack;
        break;
    case SET_DECAY:
        if (delta > 0 && decay < 15) ++decay;
        else if (delta < 0 && decay > 0) --decay;
        break;
    case SET_SUSTAIN:
        if (delta > 0 && sustain < 15) ++sustain;
        else if (delta < 0 && sustain > 0) --sustain;
        break;
    case SET_RELEASE:
        if (delta > 0 && release < 15) ++release;
        else if (delta < 0 && release > 0) --release;
        break;
    case SET_VOLUME:
        if (delta > 0 && volume < 15) ++volume;
        else if (delta < 0 && volume > 0) --volume;
        SID_VOLUME_REG = (unsigned char)(volume | (filter_enabled ? 0x10 : 0));
        break;
    case SET_FILTER:
        filter_enabled = delta > 0 ? 1 : 0;
        apply_filter();
        break;
    }
    if (note_active) note_on(current_note);
    else draw_screen();
}

static void play_key(unsigned char key)
{
    static const char keys[] = "AWSEDFTGYHUJ";
    const char *found;

    found = strchr(keys, key);
    if (found != 0) note_on((unsigned char)(found - keys));
}

static unsigned char joystick_state(void)
{
    /* CIA1 port A, active-low joystick port 2; direction bits + FIRE. */
    return (unsigned char)(JOY2_PORT & 0x1F);
}

static void handle_joystick(unsigned char state, unsigned char previous)
{
    unsigned char pressed;
    pressed = (unsigned char)(previous & (unsigned char)~state);
    if (pressed & 0x01) select_setting(-1);      /* UP */
    else if (pressed & 0x02) select_setting(1);  /* DOWN */
    if (pressed & 0x04) adjust_setting(-1);      /* LEFT */
    else if (pressed & 0x08) adjust_setting(1);  /* RIGHT */
    if (pressed & 0x10) note_on(0);              /* FIRE plays C */
}

static unsigned char exit_command(void)
{
    char command[8];
    unsigned char length;
    unsigned char ch;

    length = 0;
    command[0] = '\0';
    textcolor(COLOR_YELLOW);
    gotoxy(2, 24); cputs("SYNTH>                 ");
    gotoxy(9, 24);
    while (1) {
        if (kbhit()) {
            ch = cgetc();
            if (ch == 27) return 0;
            if (ch == 13) {
                command[length] = '\0';
                return (unsigned char)(strcmp(command, "EXIT") == 0);
            }
            if ((ch == 20 || ch == 8) && length > 0) {
                --length;
                command[length] = '\0';
                cputc(20);
            } else if (length < sizeof(command) - 1 && ch >= 32 && ch <= 126) {
                ch = (unsigned char)toupper(ch);
                command[length++] = (char)ch;
                cputc((char)ch);
            }
        }
        waitvsync();
    }
}

void paw_synth_run(void)
{
    unsigned char ch;
    unsigned char joy;
    unsigned char previous_joy;
    unsigned char done;

    sid_save();
    note_active = 0;
    selected_setting = SET_WAVE;
    joy = joystick_state();
    previous_joy = joy;
    done = 0;
    draw_screen();

    while (!done) {
        if (kbhit()) {
            ch = cgetc();
            if (ch == 27) done = 1;
            else if (ch == '/') {
                if (exit_command()) done = 1;
                else draw_screen();
            }
            if (ch == 13 && note_active == 0) continue;
            if (done) continue;
            if (ch == ' ' || ch == 3) {
                note_off();
            } else if (ch == 'Z' || ch == 'z') {
                if (octave > 2) --octave;
                if (note_active) note_on(current_note);
                else draw_screen();
            } else if (ch == 'X' || ch == 'x') {
                if (octave < 6) ++octave;
                if (note_active) note_on(current_note);
                else draw_screen();
            } else if (ch == 17) {
                select_setting(-1);
            } else if (ch == 145) {
                select_setting(1);
            } else if (ch == 157 || ch == '-') {
                adjust_setting(-1);
            } else if (ch == 29 || ch == '+') {
                adjust_setting(1);
            } else {
                play_key((unsigned char)toupper(ch));
            }
        }

        joy = joystick_state();
        handle_joystick(joy, previous_joy);
        previous_joy = joy;
        waitvsync();
    }

    SID3_CONTROL = 0;
    sid_restore();
    draw_screen();
}
