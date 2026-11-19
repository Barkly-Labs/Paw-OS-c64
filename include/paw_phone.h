#ifndef PAW_PHONE_H
#define PAW_PHONE_H

void phone_dial(const char *number, unsigned char fast);
void phone_help(void);
void modem_help(void);
void modem_dial_preview(const char *number);

#endif
