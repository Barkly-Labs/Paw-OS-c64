#ifndef PAW_HW_H
#define PAW_HW_H

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

#endif
