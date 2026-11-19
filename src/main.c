
#include <conio.h>

int main(void)
{
    clrscr();

    bgcolor(COLOR_BLUE);
    bordercolor(COLOR_BLUE);
    textcolor(COLOR_LIGHTRED);

    gotoxy(5, 2);
    cputs("********************************");

    gotoxy(10, 5);
    cputs("PAW OS");

    gotoxy(5, 7);
    cputs("BARKLY LABS C64 EDITION");

    gotoxy(5, 9);
    cputs("********************************");

    textcolor(COLOR_WHITE);

    gotoxy(5, 13);
    cputs("6510 PROCESSOR: READY");

    gotoxy(5, 15);
    cputs("64 KB MEMORY: READY");

    gotoxy(5, 17);
    cputs("PAW SYSTEM: ONLINE");

    gotoxy(5, 21);
    cputs("PRESS ANY KEY TO CONTINUE");

    cgetc();

    return 0;
}
