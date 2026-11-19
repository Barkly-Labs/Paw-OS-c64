#include <conio.h>
#include <stdlib.h>
#include "paw.h"

void show_memory(void)
{
    unsigned int total;
    unsigned int largest;

    total = _heapmemavail();
    largest = _heapmaxavail();

    textcolor(COLOR_WHITE);

    gotoxy(2, 14);
    cprintf("FREE HEAP: %u BYTES   ", total);

    gotoxy(2, 15);
    cprintf("LARGEST BLOCK: %u BYTES   ", largest);
}
