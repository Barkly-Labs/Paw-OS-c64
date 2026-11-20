#include <string.h>
#include <cbm.h>
#include "paw_dtmf.h"

/* SID voice 1 registers ($D400-$D406). */
#define SID1_FREQ_LO (*(volatile unsigned char*)0xD400)
#define SID1_FREQ_HI (*(volatile unsigned char*)0xD401)
#define SID1_CONTROL (*(volatile unsigned char*)0xD404)
#define SID1_ATTACK  (*(volatile unsigned char*)0xD405)
#define SID1_SUSTAIN (*(volatile unsigned char*)0xD406)

/* SID voice 2 registers ($D407-$D40D). */
#define SID2_FREQ_LO (*(volatile unsigned char*)0xD407)
#define SID2_FREQ_HI (*(volatile unsigned char*)0xD408)
#define SID2_CONTROL (*(volatile unsigned char*)0xD40B)
#define SID2_ATTACK  (*(volatile unsigned char*)0xD40C)
#define SID2_SUSTAIN (*(volatile unsigned char*)0xD40D)

#define SID_VOLUME (*(volatile unsigned char*)0xD418)

/*
 * DTMF frequency pairs converted to SID tuning words for a nominal NTSC
 * C64 clock (approximately 1.0227 MHz). Rows: 697, 770, 852, 941 Hz.
 * Columns: 1209, 1336, 1477, 1633 Hz.
 */
static const unsigned int low_words[4] = {
    11434, 12631, 13977, 15437
};

static const unsigned int high_words[4] = {
    19833, 21916, 24229, 26788
};

void dtmf_stop(void)
{
    /* Gate off both voices and mute the SID output. */
    SID1_CONTROL = 0x10;
    SID2_CONTROL = 0x10;
    SID_VOLUME = 0;
}

void dtmf_play(char key, unsigned char tone_frames, unsigned char gap_frames)
{
    static const char keypad[] = "123A456B789C*0#D";
    const char *found;
    unsigned char index;
    unsigned char row;
    unsigned char col;
    unsigned char frame;

    found = strchr(keypad, key);
    if (found == 0) return;

    index = (unsigned char)(found - keypad);
    row = index / 4;
    col = index % 4;

    /* Stop/reset the two voices before changing their frequencies. */
    SID1_CONTROL = 0;
    SID2_CONTROL = 0;

    /* Fast attack, short decay, maximum sustain, short release. */
    SID1_ATTACK = 0x00;
    SID1_SUSTAIN = 0xF0;
    SID2_ATTACK = 0x00;
    SID2_SUSTAIN = 0xF0;

    SID1_FREQ_LO = (unsigned char)(low_words[row] & 0xFF);
    SID1_FREQ_HI = (unsigned char)(low_words[row] >> 8);
    SID2_FREQ_LO = (unsigned char)(high_words[col] & 0xFF);
    SID2_FREQ_HI = (unsigned char)(high_words[col] >> 8);

    /* Triangle waveform (bit 4) plus gate (bit 0), on both voices. */
    SID_VOLUME = 15;
    SID1_CONTROL = 0x11;
    SID2_CONTROL = 0x11;

    for (frame = 0; frame < tone_frames; ++frame) {
        waitvsync();
    }

    /* End this digit, then leave the requested inter-digit silence. */
    SID1_CONTROL = 0x10;
    SID2_CONTROL = 0x10;
    for (frame = 0; frame < gap_frames; ++frame) {
        waitvsync();
    }

    SID_VOLUME = 0;
}

void dtmf_string(const char *sequence, unsigned char tone_frames,
                 unsigned char gap_frames)
{
    while (*sequence != '\0') {
        if ((*sequence >= '0' && *sequence <= '9') ||
            *sequence == '*' || *sequence == '#' ||
            (*sequence >= 'A' && *sequence <= 'D')) {
            dtmf_play(*sequence, tone_frames, gap_frames);
        } else if (*sequence == ',') {
            unsigned char frame;
            /* Comma inserts an approximately half-second pause at 60 Hz. */
            for (frame = 0; frame < 30; ++frame) {
                waitvsync();
            }
        }
        /* Ignore spaces, dashes, and parentheses used to format numbers. */
        ++sequence;
    }

    dtmf_stop();
}
