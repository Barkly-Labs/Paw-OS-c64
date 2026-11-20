

/* PawOS Puppy Easter Egg: disk-backed cc65 / C64 VIC-II multicolor bitmap.
 *
 * This module uses VIC bank 3 ($C000-$FFFF), bitmap at $C000 and screen
 * matrix $E000-$E3E7. build/pawos.cfg reserves $C000-$E3E7 from cc65-managed
 * memory. The bitmap and screen RAM are dedicated display storage; only Color
 * RAM and VIC/CIA configuration are saved and restored.
 */
#include <conio.h>
#include <stdint.h>
#include "paw_puppy_egg.h"
#include "paw_disk.h"

#define BITMAP_ADDR 0xC000u
#define SCREEN_ADDR 0xE000u
#define BITMAP_SIZE 8000u
#define SCREEN_SIZE 1000u
#define COLOR_ADDR  0xD800u
#define COLOR_SIZE  1000u
#define PAW_VIC(addr) (*(volatile unsigned char *)(addr))

/* The linker reserves $C000-$E3E7 exclusively for VIC-II graphics. The
 * shell's text screen is elsewhere, so the old bitmap/screen backup arrays
 * were both wasteful and impossible to read safely under ROM overlays. Keep
 * the Color RAM backup because Color RAM is shared with the shell display. */
/* Color RAM stores only four meaningful bits per cell; pack two cells per byte. */
static unsigned char saved_color[COLOR_SIZE / 2u];

/* Small IEC staging buffer; the image itself is stored on disk. */
#define EGG_IO_CHUNK 128u
static unsigned char egg_io_buffer[EGG_IO_CHUNK];
/* Color RAM uses four bits per cell, so keep the staged asset packed. */
static unsigned char egg_disk_color[COLOR_SIZE / 2u];

/* The loader writes into $C000-$DFFF before the egg display mode is selected.
 * Refuse that early write whenever the VIC is already using bank 3; this
 * conservative rule also covers text-mode character sets and sprite pointers. */
static unsigned char disk_asset_memory_safe(void)
{
    return (unsigned char)((PAW_VIC(0xdd00) & 0x03u) != 0u);
}

unsigned char paw_puppy_egg_load_bitmap(void){
    volatile unsigned char *bitmap;
    unsigned char old_01;
    unsigned char io_01;
    unsigned char ram_01;
    unsigned char hidden;
    unsigned char cell_y;
    unsigned char scanline;
    unsigned char cell_x;
    unsigned char need_hidden;
    unsigned int cell_base;
    unsigned int dst;
    unsigned int offset;
    unsigned int want;
    unsigned int i;
    int count;

    if (paw_disk_open_prg("BITMAP") != 0) return 0;
    bitmap = (volatile unsigned char *)BITMAP_ADDR;
    old_01 = PAW_VIC(0x01);
    io_01 = (unsigned char)(old_01 | 0x07u);
    /* Keep KERNAL mapped and select character ROM for reads at $D000; stores
     * still reach underlying RAM, while I/O remains unavailable to the CPU. */
    ram_01 = (unsigned char)((old_01 & 0xf8u) | 0x02u);

    offset = 0;
    cell_y = 0;
    scanline = 0;
    cell_x = 0;
    cell_base = 0;
    while (offset < BITMAP_SIZE) {
        want = BITMAP_SIZE - offset;
        if (want > EGG_IO_CHUNK) want = EGG_IO_CHUNK;

        PAW_VIC(0x01) = io_01;
        count = paw_disk_read_prg(egg_io_buffer, want);
        PAW_VIC(0x01) = old_01;
        if (count <= 0 || (unsigned int)count > want) {
            PAW_VIC(0x01) = io_01;
            paw_disk_close_prg();
            PAW_VIC(0x01) = old_01;
            return 0;
        }

        /* The file is scanline ordered; VIC bitmap RAM is cell ordered.
         * Keep the transpose counters across short reads. IRQs are masked
         * only while this bounded buffer is copied with $D000 RAM exposed. */
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
                    ++cell_y;
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
    if (count != 0) {
        paw_disk_close_prg();
        PAW_VIC(0x01) = old_01;
        return 0;
    }
    paw_disk_close_prg();
    PAW_VIC(0x01) = old_01;
    return 1;
}