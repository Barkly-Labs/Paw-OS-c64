#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <cbm.h>
#include "paw.h"
#include "paw_hw.h"
#include "paw_disk.h"
#include "paw_files.h"

#define FILES_VISIBLE_ROWS 14
#define FILES_LIST_TOP 6
#define FILES_LFN 2
#define JOY2_PORT (*(volatile unsigned char*)0xDC00)

static struct cbm_dirent directory[PAW_DISK_MAX_ENTRIES];
static unsigned char directory_count;
static unsigned char selected_entry;
static unsigned int free_blocks;
static unsigned char directory_truncated;
static char disk_message[40];

static const char *file_type_name(unsigned char type)
{
    switch (type) {
    case CBM_T_DEL: return "DEL";
    case CBM_T_SEQ: return "SEQ";
    case CBM_T_PRG: return "PRG";
    case CBM_T_USR: return "USR";
    case CBM_T_REL: return "REL";
    case CBM_T_CBM: return "CBM";
    case CBM_T_DIR: return "DIR";
    default: return "???";
    }
}

static unsigned char load_directory(void)
{
    unsigned char result;

    result = paw_disk_read_directory(directory, PAW_DISK_MAX_ENTRIES,
                                     &directory_count, &free_blocks,
                                     &directory_truncated);
    if (result != 0) {
        strncpy(disk_message, paw_disk_last_status(), sizeof(disk_message) - 1);
        disk_message[sizeof(disk_message) - 1] = '\0';
    } else {
        strcpy(disk_message, directory_truncated ? "LIST TRUNCATED AT 18 FILES" :
                                                   "DIRECTORY READ OK");
    }
    if (selected_entry >= directory_count && directory_count > 0)
        selected_entry = directory_count - 1;
    if (directory_count == 0) selected_entry = 0;
    return result;
}

static void draw_files_screen(void)
{
    unsigned char i;
    unsigned char top;
    unsigned char index;
    unsigned char row;
    unsigned char name_length;

    clrscr();
    BORDER = COLOR_PURPLE;
<<<<<<< HEAD
    BACKGROUND = COLOR_BLUE;
=======
    BACKGROUND = COLOR_BLACK;

>>>>>>> 5c1b5064ffd1b133295cda87a67dcf8484ada276
    textcolor(COLOR_YELLOW);
    gotoxy(14, 1); cputs("PAW FILES");
    textcolor(COLOR_CYAN);
    gotoxy(2, 3); cprintf("DEVICE: %u", (unsigned int)paw_disk_device());
    gotoxy(20, 3); cputs("PATH: / (DISK ROOT)");

    textcolor(COLOR_WHITE);
    gotoxy(2, 4); cputs("NAME              TYPE   BLOCKS");
    gotoxy(2, 5); cputs("--------------------------------------");

    top = 0;
    if (selected_entry >= FILES_VISIBLE_ROWS)
        top = (unsigned char)(selected_entry - FILES_VISIBLE_ROWS + 1);

    for (i = 0; i < FILES_VISIBLE_ROWS; ++i) {
        row = (unsigned char)(FILES_LIST_TOP + i);
        index = (unsigned char)(top + i);
        gotoxy(2, row);
        if (index < directory_count) {
            if (index == selected_entry) {
                textcolor(COLOR_YELLOW);
                cputc('>');
            } else {
                textcolor(COLOR_WHITE);
                cputc(' ');
            }
            cputs(directory[index].name);
            name_length = (unsigned char)strlen(directory[index].name);
            while (name_length < 16) {
                cputc(' ');
                ++name_length;
            }
            cputc(' ');
            cputs(file_type_name(directory[index].type));
            cputc(' ');
            cprintf("%u  ", (unsigned int)directory[index].size);
        } else {
            textcolor(COLOR_WHITE);
            cputs("                                      ");
        }
    }

    textcolor(COLOR_CYAN);
    gotoxy(2, 21);
    if (directory_count == 0) cputs("NO FILES FOUND");
    else cprintf("%u FILES  %u BLOCKS FREE",
                 (unsigned int)directory_count, free_blocks);
    textcolor(COLOR_WHITE);
    gotoxy(2, 22); cputs(disk_message);
    gotoxy(2, 23); cputs("UP/DOWN SELECT ENTER/T VIEW  D DELETE");
    textcolor(COLOR_YELLOW);
    gotoxy(2, 24); cputs("Q/ESC EXIT");
}

static void wait_for_key(void)
{
    while (kbhit()) cgetc();
    cgetc();
}

static void show_action_message(const char *message)
{
    textcolor(COLOR_WHITE);
    gotoxy(2, 22);
    cputs("                                      ");
    gotoxy(2, 22);
    cputs(message);
}

static void type_selected(void)
{
    if (selected_entry >= directory_count) return;
    if (directory[selected_entry].type != CBM_T_SEQ) {
        show_action_message("TYPE WORKS WITH SEQ FILES ONLY");
        return;
    }
    paw_files_type(directory[selected_entry].name);
    load_directory();
    draw_files_screen();
}

static void delete_selected(void)
{
    unsigned char answer;
    unsigned char code;

    if (selected_entry >= directory_count) return;
    textcolor(COLOR_YELLOW);
    gotoxy(2, 22); cputs("DELETE FILE? Y/N");
    gotoxy(2, 23); cputs(directory[selected_entry].name);
    answer = (unsigned char)toupper(cgetc());
    if (answer != 'Y') {
        draw_files_screen();
        return;
    }
    code = paw_disk_delete(directory[selected_entry].name);
    if (code == 0 || code == 1) {
        load_directory();
    } else {
        strncpy(disk_message, paw_disk_last_status(), sizeof(disk_message) - 1);
        disk_message[sizeof(disk_message) - 1] = '\0';
    }
    draw_files_screen();
}

static void move_selection(int delta)
{
    if (directory_count == 0) return;
    if (delta < 0) {
        if (selected_entry > 0) --selected_entry;
    } else if (selected_entry + 1 < directory_count) {
        ++selected_entry;
    }
    draw_files_screen();
}

static void handle_joystick(unsigned char state, unsigned char previous)
{
    unsigned char pressed;
    pressed = (unsigned char)(previous & (unsigned char)~state);
    if (pressed & 0x01) move_selection(-1);
    else if (pressed & 0x02) move_selection(1);
    if (pressed & 0x10) type_selected();
}

void paw_files_run(void)
{
    unsigned char ch;
    unsigned char joy;
    unsigned char previous_joy;

    selected_entry = 0;
    load_directory();
    joy = (unsigned char)(JOY2_PORT & 0x1F);
    previous_joy = joy;
    draw_files_screen();

    while (1) {
        if (kbhit()) {
            ch = (unsigned char)toupper(cgetc());
            if (ch == 27 || ch == 'Q') break;
            if (ch == 17 || ch == 'W') move_selection(-1);
            else if (ch == 145 || ch == 'S') move_selection(1);
            else if (ch == 13 || ch == 'T') type_selected();
            else if (ch == 'D') delete_selected();
        }
        joy = (unsigned char)(JOY2_PORT & 0x1F);
        handle_joystick(joy, previous_joy);
        previous_joy = joy;
        waitvsync();
    }

    draw_screen();
}

void paw_files_type(const char *filename)
{
    char buffer[64];
    unsigned char result;
    unsigned char row;
    unsigned char column;
    unsigned char i;
    unsigned char ch;
    int count;

    if (!paw_disk_filename_valid(filename)) {
        clrscr();
        textcolor(COLOR_LIGHTRED);
        gotoxy(2, 2); cputs("INVALID FILE NAME");
        wait_for_key();
        return;
    }

    result = paw_disk_open_seq(filename);
    clrscr();
    BORDER = COLOR_PURPLE;
    BACKGROUND = COLOR_BLUE;
    textcolor(COLOR_YELLOW);
    gotoxy(2, 1); cprintf("TYPE: %s", filename);
    if (result != 0) {
        textcolor(COLOR_LIGHTRED);
        gotoxy(2, 3); cputs(paw_disk_last_status());
        textcolor(COLOR_WHITE);
        gotoxy(2, 24); cputs("PRESS A KEY TO RETURN");
        wait_for_key();
        return;
    }

    textcolor(COLOR_WHITE);
    row = 3;
    column = 2;
    while (1) {
        count = paw_disk_read_seq(buffer, sizeof(buffer));
        if (count < 0) {
            textcolor(COLOR_LIGHTRED);
            gotoxy(2, 24); cputs("READ ERROR - PRESS A KEY");
            wait_for_key();
            break;
        }
        if (count == 0) break;
        for (i = 0; i < (unsigned char)count; ++i) {
            ch = (unsigned char)buffer[i];
            if (ch == 13) {
                ++row;
                column = 2;
                gotoxy(column, row);
            } else if (ch >= 32 && ch <= 126) {
                cputc((char)ch);
                ++column;
            } else if (ch >= 193 && ch <= 218) {
                cputc((char)(ch - 128));
                ++column;
            }
            if (column >= 40) {
                ++row;
                column = 2;
                gotoxy(column, row);
            }
            if (row >= 22) {
                textcolor(COLOR_CYAN);
                gotoxy(2, 24); cputs("MORE: KEY  STOP: Q/ESC");
                ch = (unsigned char)toupper(cgetc());
                if (ch == 'Q' || ch == 27) {
                    paw_disk_close_seq();
                    return;
                }
                clrscr();
                BORDER = COLOR_PURPLE;
                BACKGROUND = COLOR_BLUE;
                textcolor(COLOR_YELLOW);
                gotoxy(2, 1); cprintf("TYPE: %s (CONT.)", filename);
                textcolor(COLOR_WHITE);
                row = 3;
                column = 2;
            }
        }
    }
    paw_disk_close_seq();
    textcolor(COLOR_CYAN);
    gotoxy(2, 24); cputs("END OF FILE - PRESS A KEY");
    wait_for_key();
}
