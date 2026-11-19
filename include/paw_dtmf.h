#ifndef PAW_DTMF_H
#define PAW_DTMF_H

void dtmf_play(char key, unsigned char tone_frames, unsigned char gap_frames);
void dtmf_string(const char *sequence, unsigned char tone_frames, unsigned char gap_frames);

#endif
