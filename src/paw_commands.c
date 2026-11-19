#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "paw.h"
#include "paw_phone.h"
#include "paw_synth.h"
#include "paw_disk.h"
#include "paw_files.h"

static void run_phone_command(char *command)
{
    char *number;
    unsigned char fast;

    number = command;
    while (*number && *number != ' ') ++number;
    if (*number == ' ') *number++ = '\0';
    while (*number == ' ') ++number;
    if (*number == '\0') { phone_help(); return; }

    fast = 0;
    {
        char *option = number;
        while (*option && *option != ' ') ++option;
        if (*option == ' ') {
            *option++ = '\0';
            while (*option == ' ') ++option;
            if (strcmp(option, "FAST") == 0) fast = 1;
        }
    }

    /* Both DIAL and DTMF generate SID audio; DIAL is the friendly alias. */
    phone_dial(number, fast);
}

static void show_disk_status(void)
{
    char status[40];
    unsigned char code;

    code = paw_disk_read_status(status, sizeof(status));
    textcolor(COLOR_CYAN);
    gotoxy(2, 21);
    cprintf("ACTIVE DEVICE: %u", (unsigned int)paw_disk_device());
    textcolor((code & 0x80) ? COLOR_LIGHTRED : COLOR_WHITE);
    gotoxy(2, 22); cputs(status);
    if ((code & 0x80) && code != 0xFD && code != 0xFE) {
        gotoxy(2, 23); cprintf("CC65 I/O ERROR: %u", (unsigned int)(code & 0x7F));
    }
}

static void show_disk_operation(const char *label, unsigned char code)
{
    textcolor((code == 0 || code == 1) ? COLOR_LIGHTGREEN : COLOR_LIGHTRED);
    gotoxy(2, 21); cputs(label);
    gotoxy(2, 22);
    if (code == 0xFD) cputs("INVALID OR UNSUPPORTED FILE NAME");
    else cputs(paw_disk_last_status());
    if ((code & 0x80) && code != 0xFD && code != 0xFE) {
        gotoxy(2, 23); cprintf("CC65 I/O ERROR: %u", (unsigned int)(code & 0x7F));
    }
}

static void run_disk_selection(char *argument)
{
    unsigned char device;

    while (*argument == ' ') ++argument;
    if ((argument[0] == '8' || argument[0] == '9') && argument[1] == '\0') {
        device = (unsigned char)(argument[0] - '0');
    } else if (argument[0] == '1' &&
               (argument[1] == '0' || argument[1] == '1') &&
               argument[2] == '\0') {
        device = (unsigned char)(10 + argument[1] - '0');
    } else {
        textcolor(COLOR_LIGHTRED);
        gotoxy(2, 21); cputs("USE DISK 8 THROUGH DISK 11");
        return;
    }
    paw_disk_set_device(device);
    show_disk_status();
}

static void run_disk_delete(char *filename)
{
    unsigned char answer;
    unsigned char code;

    while (*filename == ' ') ++filename;
    if (!paw_disk_filename_valid(filename)) {
        show_disk_operation("DELETE NOT STARTED", 0xFD);
        return;
    }
    textcolor(COLOR_YELLOW);
    gotoxy(2, 21); cputs("DELETE FILE? PRESS Y TO CONFIRM");
    gotoxy(2, 22); cputs(filename);
    gotoxy(2, 23); cputs("ANY OTHER KEY CANCELS");
    answer = (unsigned char)toupper(cgetc());
    if (answer != 'Y') {
        gotoxy(2, 23); cputs("DELETE CANCELLED");
        return;
    }
    code = paw_disk_delete(filename);
    show_disk_operation("DELETE RESULT", code);
}

static void run_disk_rename(char *arguments)
{
    char *old_name;
    char *new_name;
    char *separator;
    unsigned char code;

    while (*arguments == ' ') ++arguments;
    old_name = arguments;
    separator = old_name;
    while (*separator && *separator != ' ') ++separator;
    if (*separator == '\0') {
        show_disk_operation("USE RENAME OLD NEW", 0xFD);
        return;
    }
    *separator++ = '\0';
    while (*separator == ' ') ++separator;
    new_name = separator;
    while (*separator && *separator != ' ') ++separator;
    if (*separator != '\0') {
        *separator = '\0';
        while (*++separator == ' ') { }
        if (*separator != '\0') {
            show_disk_operation("USE RENAME OLD NEW", 0xFD);
            return;
        }
    }
    code = paw_disk_rename(old_name, new_name);
    show_disk_operation("RENAME RESULT", code);
}

static void show_safe_loader_limit(const char *message)
{
    textcolor(COLOR_YELLOW);
    gotoxy(2, 21); cputs(message);
    textcolor(COLOR_WHITE);
    gotoxy(2, 22); cputs("NO SAFE RESIDENT APP MEMORY MAP YET");
}

void execute_command(char *command)
{
    char *argument;
    clear_output();

    if (strcmp(command, "HELP") == 0) show_help();
    else if (strcmp(command, "MEM") == 0) {
        show_memory(); textcolor(COLOR_CYAN); gotoxy(2, 21); cputs("MEMORY STATUS UPDATED");
    }
    else if (strcmp(command, "ABOUT") == 0) show_about();
    else if (strcmp(command, "CLS") == 0) draw_screen();
    else if (strcmp(command, "SYNTH") == 0) paw_synth_run();
    else if (strcmp(command, "DIR") == 0 || strcmp(command, "FILES") == 0)
        paw_files_run();
    else if (strcmp(command, "DISK") == 0) show_disk_status();
    else if (strncmp(command, "DISK ", 5) == 0)
        run_disk_selection(command + 5);
    else if (strcmp(command, "PWD") == 0) {
        textcolor(COLOR_CYAN); gotoxy(2, 21);
        cprintf("DEVICE %u: / (COMMODORE DISK ROOT)",
                (unsigned int)paw_disk_device());
    }
    else if (strcmp(command, "CD") == 0 || strcmp(command, "CD ..") == 0) {
        textcolor(COLOR_CYAN); gotoxy(2, 21);
        cputs("CURRENT CONTEXT: DEVICE ROOT /");
        gotoxy(2, 22); cputs("1541 DOS HAS NO NATIVE SUBDIRECTORIES");
    }
    else if (strncmp(command, "CD ", 3) == 0) {
        textcolor(COLOR_YELLOW); gotoxy(2, 21);
        cputs("NAVIGATION IS AT THE DISK ROOT ONLY");
    }
    else if (strncmp(command, "TYPE ", 5) == 0) {
        paw_files_type(command + 5);
        draw_screen();
    }
    else if (strncmp(command, "DELETE ", 7) == 0)
        run_disk_delete(command + 7);
    else if (strncmp(command, "RENAME ", 7) == 0)
        run_disk_rename(command + 7);
    else if (strcmp(command, "LOAD") == 0 || strncmp(command, "LOAD ", 5) == 0)
        show_safe_loader_limit("LOAD DISABLED FOR MEMORY SAFETY");
    else if (strcmp(command, "RUN") == 0)
        show_safe_loader_limit("RUN DISABLED FOR MEMORY SAFETY");
    else if (strcmp(command, "SAVE") == 0 || strncmp(command, "SAVE ", 5) == 0)
        show_safe_loader_limit("SAVE NEEDS A DEFINED SOURCE REGION");
    else if (strcmp(command, "COPY") == 0 || strncmp(command, "COPY ", 5) == 0)
        show_safe_loader_limit("COPY IS NOT ENABLED IN THIS BUILD");
    else if (strcmp(command, "DTMFTEST") == 0) phone_dtmf_test();
    else if (strncmp(command, "DTMF ", 5) == 0) run_phone_command(command + 5);
    else if (strncmp(command, "DIAL ", 5) == 0) run_phone_command(command + 5);
    else if (strcmp(command, "PHONE") == 0) phone_help();
    else if (strcmp(command, "MODEM") == 0) modem_help();
    else if (strncmp(command, "MODEM DIAL ", 11) == 0) {
        argument = command + 11;
        modem_dial_preview(argument);
    }
    else if (command[0] != '\0') {
        textcolor(COLOR_LIGHTRED); gotoxy(2, 21); cputs("UNKNOWN COMMAND");
    }
}
