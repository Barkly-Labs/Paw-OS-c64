#include <conio.h>
#include <cbm.h>
#include "paw_hw.h"
#include "paw.h"


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

/* Generated from user-provided MIDI: first 30 seconds, 60 Hz frame grid.
   Guitar notes occupy voices 1/2; bass occupies voice 3.
   Note lanes are simplified to one note per SID voice. */

#define PAW_SID2_FREQ_LO (*(volatile unsigned char*)0xD407)
#define PAW_SID2_FREQ_HI (*(volatile unsigned char*)0xD408)
#define PAW_SID2_CONTROL (*(volatile unsigned char*)0xD40B)
#define PAW_SID2_AD (*(volatile unsigned char*)0xD40C)
#define PAW_SID2_SR (*(volatile unsigned char*)0xD40D)
#define PAW_SID3_FREQ_LO (*(volatile unsigned char*)0xD40E)
#define PAW_SID3_FREQ_HI (*(volatile unsigned char*)0xD40F)
#define PAW_SID3_CONTROL (*(volatile unsigned char*)0xD412)
#define PAW_SID3_AD (*(volatile unsigned char*)0xD413)
#define PAW_SID3_SR (*(volatile unsigned char*)0xD414)

typedef struct { unsigned int frames; unsigned int v1; unsigned int v2; unsigned int v3; } paw_sid_event_t;

void play_midi_sid(void)
{
    static const paw_sid_event_t music[] = {
        /* frames, SID frequency word voice 1, voice 2, voice 3 */
        {51, 0, 0, 0},
        {44, 11226, 4720, 1180},
        {46, 11226, 4720, 3536},
        {45, 9440, 7493, 937},
        {45, 9440, 7493, 3746},
        {45, 14144, 8410, 2807},
        {45, 14144, 8410, 4205},
        {89, 12601, 8910, 3536},
        {90, 11226, 9440, 2360},
        {44, 9440, 7493, 1873},
        {45, 9440, 7493, 0},
        {90, 14144, 8410, 0},
        {89, 12601, 8910, 0},
        {89, 11226, 9440, 590},
        {90, 11226, 9440, 468},
        {55, 14144, 8410, 702},
        {23, 14144, 8410, 884},
        {11, 14144, 8410, 702},
        {90, 10596, 8910, 884},
        {89, 11226, 9440, 590},
        {45, 9440, 7493, 468},
        {45, 9440, 7493, 3536},
        {66, 14144, 8410, 702},
        {23, 14144, 8410, 884},
        {89, 10596, 8910, 884},
        {89, 11226, 9440, 590},
        {90, 11226, 9440, 468},
        {45, 14144, 8410, 702},
        {11, 8410, 7072, 702},
        {12, 8410, 7072, 884},
        {10, 14144, 8410, 884},
        {12, 14144, 8410, 702},
        {48, 12601, 8910, 884},
    };
    unsigned int i, f;
    SID_VOLUME = 15;
    SID_ATTACK = 0x02; SID_SUSTAIN = 0xF8;
    PAW_SID2_AD = 0x02; PAW_SID2_SR = 0xF8;
    PAW_SID3_AD = 0x02; PAW_SID3_SR = 0xF8;
    SID_CONTROL = 0; PAW_SID2_CONTROL = 0; PAW_SID3_CONTROL = 0;
    for (i = 0; i < sizeof(music)/sizeof(music[0]); ++i) {
        SID_FREQ_LO = music[i].v1 & 0xFF; SID_FREQ_HI = music[i].v1 >> 8;
        PAW_SID2_FREQ_LO = music[i].v2 & 0xFF; PAW_SID2_FREQ_HI = music[i].v2 >> 8;
        PAW_SID3_FREQ_LO = music[i].v3 & 0xFF; PAW_SID3_FREQ_HI = music[i].v3 >> 8;
        SID_CONTROL = music[i].v1 ? 0x11 : 0x10;
        PAW_SID2_CONTROL = music[i].v2 ? 0x11 : 0x10;
        PAW_SID3_CONTROL = music[i].v3 ? 0x11 : 0x10;
        for (f = 0; f < music[i].frames; ++f) waitvsync();
    }
    SID_CONTROL = 0x10; PAW_SID2_CONTROL = 0x10; PAW_SID3_CONTROL = 0x10;
    SID_VOLUME = 0;
}