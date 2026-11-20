#include <conio.h>
#include <cbm.h>
#include "paw_hw.h"
#include "paw.h"


/* 30-second MIDI-derived SID sequence; event frequencies are SID register values. */
typedef struct {
    unsigned int frames;
    unsigned int v1;
    unsigned int v2;
    unsigned int v3;
} paw_sid_event_t;


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

void play_midi_sid(void)
{
    static const paw_sid_event_t music[] = {
        /* duration_frames, voice1_frequency, voice2_frequency, voice3_frequency */
    {51, 0, 0, 0},
    {44, 4720, 1180, 0},
    {46, 3536, 3536, 0},
    {45, 7493, 937, 0},
    {45, 3746, 3746, 0},
    {45, 8410, 2807, 0},
    {45, 4205, 4205, 0},
    {44, 8910, 3536, 0},
    {45, 5298, 3536, 0},
    {44, 9440, 2360, 0},
    {46, 3536, 2360, 0},
    {44, 7493, 1873, 0},
    {45, 3746, 0, 0},
    {45, 8410, 0, 0},
    {45, 4205, 0, 0},
    {46, 8910, 0, 0},
    {43, 5298, 0, 0},
    {89, 4720, 590, 0},
    {90, 7493, 468, 0},
    {55, 8410, 702, 0},
    {12, 8410, 884, 0},
    {11, 4205, 884, 0},
    {11, 4205, 702, 0},
    {23, 4455, 884, 0},
    {22, 5613, 884, 0},
    {23, 5298, 884, 0},
    {22, 7072, 884, 0},
    {67, 4720, 590, 0},
    {22, 6300, 590, 0},
    {45, 4720, 468, 0},
    {45, 3746, 3536, 0},
    {66, 7072, 702, 0},
    {23, 4205, 884, 0},
    {45, 6300, 884, 0},
    {22, 5298, 5298, 0},
    {22, 5298, 884, 0},
    {89, 4720, 590, 0},
    {56, 7493, 468, 0},
    {12, 7493, 3536, 0},
    {22, 7493, 468, 0},
    {23, 5613, 702, 0},
    {11, 5613, 3536, 0},
    {11, 5613, 702, 0},
    {11, 5613, 3536, 0},
    {12, 5613, 884, 0},
    {10, 5613, 3150, 0},
    {12, 5613, 702, 0},
    {48, 5298, 884, 0},
    };
    unsigned char i;
    unsigned int frame;

    SID_VOLUME = 15;
    SID_CONTROL = 0;
    SID2_CONTROL = 0;
    SID3_CONTROL = 0;

    SID_ATTACK = 0x02;
    SID_SUSTAIN = 0xF8;
    SID2_ATTACK = 0x02;
    SID2_SUSTAIN = 0xF8;
    SID3_ATTACK = 0x02;
    SID3_SUSTAIN = 0xF8;

    for (i = 0; i < sizeof(music) / sizeof(music[0]); ++i) {
        SID_FREQ_LO = music[i].v1 & 0xFF;
        SID_FREQ_HI = music[i].v1 >> 8;
        SID2_FREQ_LO = music[i].v2 & 0xFF;
        SID2_FREQ_HI = music[i].v2 >> 8;
        SID3_FREQ_LO = music[i].v3 & 0xFF;
        SID3_FREQ_HI = music[i].v3 >> 8;

        SID_CONTROL = music[i].v1 ? 0x11 : 0x10;
        SID2_CONTROL = music[i].v2 ? 0x11 : 0x10;
        SID3_CONTROL = music[i].v3 ? 0x11 : 0x10;

        for (frame = 0; frame < music[i].frames; ++frame)
            waitvsync();
    }

    SID_CONTROL = 0x10;
    SID2_CONTROL = 0x10;
    SID3_CONTROL = 0x10;
    SID_VOLUME = 0;
}

