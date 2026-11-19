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
