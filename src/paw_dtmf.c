#include <string.h>
#include <cbm.h>
#include "paw_dtmf.h"

/* SID voice 1 and voice 2 registers */
#define SID1_FREQ_LO (*(volatile unsigned char*)0xD400)
#define SID1_FREQ_HI (*(volatile unsigned char*)0xD401)
#define SID1_CONTROL (*(volatile unsigned char*)0xD404)
#define SID2_FREQ_LO (*(volatile unsigned char*)0xD407)
#define SID2_FREQ_HI (*(volatile unsigned char*)0xD408)
#define SID2_CONTROL (*(volatile unsigned char*)0xD40B)
#define SID_VOLUME   (*(volatile unsigned char*)0xD418)

/* DTMF frequencies converted to SID tuning words for a nominal NTSC clock. */
static const unsigned int low_words[4] = {
    11434, 12631, 13977, 15437
};
static const unsigned int high_words[4] = {
    19833, 21916, 24229, 26788
};

void dtmf_play(char key, unsigned char tone_frames, unsigned char gap_frames)
{
    static const char keypad[] = "123A456B789C*0#D";
    const char *found;
    unsigned char index, row, col, frame;

    found = strchr(keypad, key);
    if (found == 0) return;

    index = (unsigned char)(found - keypad);
    row = index / 4;
    col = index % 4;

    SID1_CONTROL = 0;
    SID2_CONTROL = 0;
    SID_VOLUME = 12;
    SID1_FREQ_LO = (unsigned char)(low_words[row] & 0xFF);
    SID1_FREQ_HI = (unsigned char)(low_words[row] >> 8);
    SID2_FREQ_LO = (unsigned char)(high_words[col] & 0xFF);
    SID2_FREQ_HI = (unsigned char)(high_words[col] >> 8);
    SID1_CONTROL = 0x11;
    SID2_CONTROL = 0x11;

    for (frame = 0; frame < tone_frames; ++frame) waitvsync();

    SID1_CONTROL = 0x10;
    SID2_CONTROL = 0x10;
    for (frame = 0; frame < gap_frames; ++frame) waitvsync();
    SID_VOLUME = 0;
}

void dtmf_string(const char *sequence, unsigned char tone_frames, unsigned char gap_frames)
{
    while (*sequence != '\0') {
        if (*sequence >= '0' && *sequence <= '9') {
            dtmf_play(*sequence, tone_frames, gap_frames);
        } else if (*sequence == '*' || *sequence == '#' ||
                   (*sequence >= 'A' && *sequence <= 'D')) {
            dtmf_play(*sequence, tone_frames, gap_frames);
        } else if (*sequence == ',') {
            unsigned char frame;
            for (frame = 0; frame < 30; ++frame) waitvsync();
        }
        /* Spaces, dashes, and parentheses are formatting only. */
        ++sequence;
    }
}
