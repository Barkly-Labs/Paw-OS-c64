#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "paw.h"

int main(void)
{
    char command[40];
    unsigned char length;
    unsigned char ch;

    draw_screen();

    while (1) {

        gotoxy(2, 24);
        cputs("PAW>                  ");
        gotoxy(6, 24);

        length = 0;
        command[0] = '\0';

        while (1) {
            ch = cgetc();

            if (ch == 13) {
                command[length] = '\0';
                break;
            }

            if (ch == 20 && length > 0) {
                length--;
                command[length] = '\0';
                cputc(20);
            }
            else if (ch >= 32 && ch <= 126 &&
                     length < sizeof(command) - 1) {

                ch = toupper(ch);
                command[length++] = ch;
                cputc(ch);
            }
        }

        execute_command(command);
    }

    return 0;
}
