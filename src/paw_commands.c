#include <conio.h>
#include <string.h>
#include "paw.h"
#include "paw_phone.h"

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
