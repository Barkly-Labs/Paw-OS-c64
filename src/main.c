
#include <conio.h>
#include <stdio.h>

int main(void)
{
    clrscr();

    textcolor(COLOR_LIGHTRED);
    cprintf("********************************\n");
    cprintf("*                              *\n");
    cprintf("*          PAW OS              *\n");
    cprintf("*                              *\n");
    cprintf("*   BARKLY LABS C64 EDITION    *\n");
    cprintf("*                              *\n");
    cprintf("********************************\n\n");

    textcolor(COLOR_WHITE);
    cprintf("6510 PROCESSOR: ONLINE\n");
    cprintf("64 KB MEMORY: READY\n");
    cprintf("PAW SYSTEM: INITIALIZING\n\n");

    cprintf("PRESS ANY KEY TO CONTINUE...");

    cgetc();

    return 0;
}