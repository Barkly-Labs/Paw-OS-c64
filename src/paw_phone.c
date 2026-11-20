#include <conio.h>
#include <string.h>
#include "paw_dtmf.h"
#include "paw_phone.h"

void phone_dial(const char *number, unsigned char fast)
{
    textcolor(COLOR_CYAN);
    gotoxy(2, 21);
    cputs("SID DTMF AUDIO: ");
    cputs(number);

    textcolor(COLOR_WHITE);
    gotoxy(2, 22);
    cputs("GENERATING TONES...                ");

    if (fast) dtmf_string(number, 7, 3);
    else dtmf_string(number, 12, 5);

    textcolor(COLOR_LIGHTGREEN);
    gotoxy(2, 23);
    cputs("TONE SEQUENCE COMPLETE             ");
}

void phone_help(void)
{
    textcolor(COLOR_CYAN);
    gotoxy(2, 21); cputs("PHONE COMMANDS");
    textcolor(COLOR_WHITE);
    gotoxy(2, 22); cputs("DIAL number [FAST]");
    gotoxy(2, 23); cputs("DTMF number [FAST]");
}

void modem_help(void)
{
    textcolor(COLOR_CYAN);
    gotoxy(2, 21); cputs("DIAL-UP MODEM");
    textcolor(COLOR_WHITE);
    gotoxy(2, 22); cputs("NEEDS RS232 MODEM + C64 SERIAL ADAPTER");
    gotoxy(2, 23); cputs("MODEM DIAL number  (DRIVER NOT YET SET)");
}

void modem_dial_preview(const char *number)
{
    textcolor(COLOR_CYAN);
    gotoxy(2, 21); cputs("MODEM DIAL REQUEST");
    textcolor(COLOR_WHITE);
    gotoxy(2, 22); cputs("ATDT");
    cputs(number);
    textcolor(COLOR_LIGHTRED);
    gotoxy(2, 23); cputs("NOT SENT: SERIAL MODEM DRIVER REQUIRED");
}

void phone_dtmf_test(void)
{
    textcolor(COLOR_CYAN);
    gotoxy(2, 21);
    cputs("DTMF KEYPAD TEST");
    textcolor(COLOR_WHITE);
    gotoxy(2, 22);
    cputs("PLAYING 123A 456B 789C *0#D");
    dtmf_string("123A456B789C*0#D", 12, 5);
    textcolor(COLOR_LIGHTGREEN);
    gotoxy(2, 23);
    cputs("DTMF TEST COMPLETE                 ");
}
