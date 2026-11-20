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
<<<<<<< HEAD
    
    gotoxy(2, 14);
=======

    gotoxy(2, 15);
>>>>>>> 5c1b5064ffd1b133295cda87a67dcf8484ada276
    cprintf("FREE HEAP: %u BYTES   ", total);

    gotoxy(2, 16);
    cprintf("LARGEST BLOCK: %u BYTES   ", largest);
}
