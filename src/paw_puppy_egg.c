
/* PawOS Puppy Easter Egg: disk-backed cc65 / C64 VIC-II multicolor bitmap. */
#include <conio.h>
#include <stdint.h>
#include "paw_puppy_egg.h"
#include "paw_disk.h"
#include "paw_hw.h"
#include "paw_ui.h"

#define BITMAP_ADDR 0xC000u
#define BITMAP_SIZE 8000u
#define COLOR_ADDR  0xD800u
#define COLOR_SIZE  1000u
#define PAW_VIC(addr) (*(volatile unsigned char *)(addr))

#define EGG_IO_CHUNK 128u

static unsigned char saved_color[COLOR_SIZE / 2u];
static unsigned char egg_io_buffer[EGG_IO_CHUNK];
static unsigned char egg_disk_color[COLOR_SIZE / 2u];



/* Keep diagnostic messages on screen while the loader returns. */
static void egg_error_screen(const char *message)
{
    clrscr();
    rainbow();
    textcolor(COLOR_LIGHTRED);
    gotoxy(16, 11);
    cputs("PAW OS");
    textcolor(COLOR_WHITE);
    gotoxy(2, 13);
    cputs(message);
}

/* Load BITMAP from disk and transpose scanline data into VIC bitmap layout. */
unsigned char paw_puppy_egg_load_bitmap(void)
{
    volatile unsigned char *bitmap;
    unsigned char old_01;
    unsigned char io_01;
    unsigned char ram_01;
    unsigned char hidden;
    unsigned char scanline;
    unsigned char cell_x;
    unsigned char need_hidden;
    unsigned int cell_base;
    unsigned int dst;
    unsigned int offset;
    unsigned int want;
    unsigned int i;
    int count;


    if (paw_disk_open_prg("BITMAP") != 0) {
        egg_error_screen("BITMAP OPEN FAILED");
        gotoxy(2, 15);
        cputs(paw_disk_last_status());
        gotoxy(2, 16);
        cprintf("CODE: %u", paw_disk_last_code());
        return 0;
    }

    bitmap = (volatile unsigned char *)BITMAP_ADDR;

    old_01 = PAW_VIC(0x01);
    io_01 = (unsigned char)(old_01 | 0x07u);
    ram_01 = (unsigned char)((old_01 & 0xf8u) | 0x02u);

    offset = 0;
    scanline = 0;
    cell_x = 0;
    cell_base = 0;

    while (offset < BITMAP_SIZE) {
        want = BITMAP_SIZE - offset;

        if (want > EGG_IO_CHUNK)
            want = EGG_IO_CHUNK;

        PAW_VIC(0x01) = io_01;
        count = paw_disk_read_prg(egg_io_buffer, want);
        PAW_VIC(0x01) = old_01;

        if (count <= 0 || (unsigned int)count > want) {
            PAW_VIC(0x01) = old_01;
            paw_disk_close_prg();

            egg_error_screen("BITMAP READ FAILED");
            gotoxy(2, 15);
            cprintf("OFFSET: %u", offset);
            gotoxy(2, 16);
            cprintf("COUNT: %d", count);
            return 0;
        }

        asm("sei");

        PAW_VIC(0x01) = io_01;
        hidden = 0;

        for (i = 0; i < (unsigned int)count; ++i) {
            dst = cell_base + (unsigned int)cell_x * 8u + scanline;

            need_hidden = (unsigned char)(dst >= 0x1000u);

            if (need_hidden != hidden) {
                PAW_VIC(0x01) = need_hidden ? ram_01 : io_01;
                hidden = need_hidden;
            }

            bitmap[dst] = egg_io_buffer[i];

            ++cell_x;

            if (cell_x == 40u) {
                cell_x = 0;
                ++scanline;

                if (scanline == 8u) {
                    scanline = 0;
                    cell_base += 320u;
                }
            }
        }

        PAW_VIC(0x01) = old_01;
        asm("cli");

        offset += (unsigned int)count;
    }

    PAW_VIC(0x01) = io_01;
    count = paw_disk_read_prg(egg_io_buffer, 1u);
    PAW_VIC(0x01) = old_01;

    if (count != 0) {
        paw_disk_close_prg();

        egg_error_screen("BITMAP SIZE INVALID");
        gotoxy(2, 15);
        cputs("EXPECTED 8000 BYTES");
        return 0;
    }

    paw_disk_close_prg();

    return 1;
}
