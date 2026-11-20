#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <cbm.h>
#include "paw_edit.h"
#include "paw_disk.h"
#include "paw_hw.h"
#include "paw_ui.h"

#define EDIT_SIZE 512
#define EDIT_LFN 3
#define EDIT_ROWS 18
#define EDIT_COLS 40

static char document[EDIT_SIZE];
static unsigned int length;
static unsigned int edit_pos;

static void edit_render(void)
{
    unsigned int i;
    unsigned int pos;
    unsigned char row;
    unsigned char col;

    clrscr();
    BORDER = COLOR_PURPLE;
    BACKGROUND = COLOR_BLACK;

    textcolor(COLOR_LIGHTRED);
    gotoxy(13, 1);
    cputs("PAW-EDIT");

    textcolor(COLOR_CYAN);
    gotoxy(2, 2);
    cputs("BARKLY LABS TEXT EDITOR");

    textcolor(COLOR_WHITE);
    for (row = 0; row < EDIT_ROWS; ++row) {
        gotoxy(1, row + 4);
        cputs("                                        ");
    }

    row = 0;
    col = 0;
    pos = 0;

    while (pos < length && row < EDIT_ROWS) {
        if (document[pos] == '\n') {
            ++row;
            col = 0;
        } else {
            gotoxy(col + 1, row + 4);
            cputc(document[pos]);
            ++col;
            if (col >= EDIT_COLS) {
                col = 0;
                ++row;
            }
        }
        ++pos;
    }

    textcolor(COLOR_CYAN);
    gotoxy(1, 23);
    cputs("F1 SAVE  F3 LOAD  ESC EXIT");

    textcolor(COLOR_LIGHTRED);
    gotoxy(1, 24);
    cprintf("BYTES: %u / %u", length, EDIT_SIZE);

    row = 0;
    col = 0;
    pos = 0;

    while (pos < edit_pos && row < EDIT_ROWS) {
        if (document[pos] == '\n') {
            ++row;
            col = 0;
        } else {
            ++col;
            if (col >= EDIT_COLS) {
                col = 0;
                ++row;
            }
        }
        ++pos;
    }

    if (row < EDIT_ROWS) {
        gotoxy(col + 1, row + 4);
    }
}

static void edit_filename(char *name)
{
    unsigned char count;
    unsigned char ch;

    count = 0;
    name[0] = '\0';

    textcolor(COLOR_YELLOW);
    gotoxy(1, 22);
    cputs("FILENAME:                ");
    gotoxy(11, 22);

    while (1) {
        ch = cgetc();

        if (ch == 13) break;

        if (ch == 20 && count > 0) {
            --count;
            name[count] = '\0';
            cputc(20);
        } else if (ch >= 32 && ch <= 126 && count < 16) {
            ch = toupper(ch);
            name[count++] = ch;
            cputc(ch);
        }
    }

    name[count] = '\0';
}

static void edit_save(void)
{
    char name[17];
    int written;
    unsigned char error;

    edit_filename(name);
    if (!paw_disk_filename_valid(name)) {
        textcolor(COLOR_LIGHTRED);
        gotoxy(1, 22);
        cputs("INVALID FILENAME");
        cgetc();
        return;
    }

    error = cbm_open(EDIT_LFN, paw_disk_device(),
                     CBM_WRITE, name);

    if (error != 0) {
        textcolor(COLOR_LIGHTRED);
        gotoxy(1, 22);
        cputs("DISK OPEN ERROR");
        cgetc();
        return;
    }

    written = cbm_write(EDIT_LFN, document, length);
    cbm_close(EDIT_LFN);

    textcolor(COLOR_LIGHTGREEN);
    gotoxy(1, 22);

    if (written == (int)length)
        cputs("DOCUMENT SAVED");
    else
        cputs("WRITE ERROR");

    cgetc();
}

static void edit_load(void)
{
    char name[17];
    int count;
    unsigned char error;

    edit_filename(name);

    if (!paw_disk_filename_valid(name)) {
        textcolor(COLOR_LIGHTRED);
        gotoxy(1, 22);
        cputs("INVALID FILENAME");
        cgetc();
        return;
    }

    error = paw_disk_open_seq(name);

    if (error != 0) {
        textcolor(COLOR_LIGHTRED);
        gotoxy(1, 22);
        cputs("FILE OPEN ERROR");
        cgetc();
        return;
    }

    count = paw_disk_read_seq(document, EDIT_SIZE);
    paw_disk_close_seq();

    if (count < 0) count = 0;
    if (count > EDIT_SIZE) count = EDIT_SIZE;

    length = (unsigned int)count;
    edit_pos = length;

    textcolor(COLOR_LIGHTGREEN);
    gotoxy(1, 22);
    cputs("DOCUMENT LOADED");
    cgetc();
}

void paw_edit_run(const char *filename)
{
    unsigned char ch;
    unsigned int i;

    length = 0;
    edit_pos = 0;

    if (filename != NULL && filename[0] != '\0') {
        if (paw_disk_open_seq(filename) == 0) {
            int count = paw_disk_read_seq(document, EDIT_SIZE);
            paw_disk_close_seq();

            if (count > 0) {
                length = (unsigned int)count;
                if (length > EDIT_SIZE) length = EDIT_SIZE;
                edit_pos = length;
            }
        }
    }

    edit_render();

    while (1) {
        ch = cgetc();

        if (ch == 27) break;

        if (ch == 0x85) {
            edit_save();
            edit_render();
        }
        else if (ch == 0x86) {
            edit_load();
            edit_render();
        }
        else if (ch == 0x9D && edit_pos > 0) {
            --edit_pos;
            edit_render();
        }
        else if (ch == 0x1D && edit_pos < length) {
            ++edit_pos;
            edit_render();
        }
        else if (ch == 0x91) {
            for (i = edit_pos; i > 0; --i) {
                if (document[i - 1] == '\n') {
                    edit_pos = i - 1;
                    break;
                }
            }
            edit_render();
        }
        else if (ch == 0x11) {
            for (i = edit_pos; i < length; ++i) {
                if (document[i] == '\n') {
                    edit_pos = i + 1;
                    break;
                }
            }
            edit_render();
        }
        else if (ch == 20 && edit_pos > 0) {
            for (i = edit_pos - 1; i < length - 1; ++i)
                document[i] = document[i + 1];

            --edit_pos;
            --length;
            edit_render();
        }
        else if (ch == 0x14 && edit_pos < length) {
            for (i = edit_pos; i < length - 1; ++i)
                document[i] = document[i + 1];

            --length;
            edit_render();
        }
        else if ((ch == 13 || (ch >= 32 && ch <= 126)) &&
                 length < EDIT_SIZE) {

            if (ch == 13) ch = '\n';

            for (i = length; i > edit_pos; --i)
                document[i] = document[i - 1];

            document[edit_pos] = ch;
            ++edit_pos;
            ++length;
            edit_render();
        }
    }

    draw_screen();
}
