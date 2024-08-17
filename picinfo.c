#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "selfie.h"

// This is enough for my personal picture.
#define MAX_ROWS 100
#define MAX_COL 3000

/*
This little program counts the chars(plus escape sequences), -
rows and the maximum of chars per row of the picture in 'ansi_pic'.
It then stores the picture in a 2D array and prints it out on the right side of the information.*/

int main(void)
{
    size_t l = strlen(ansi_pic);
    int maxcol = 0, row = 0, col = 0, count = 0, printable = 0, p = 0;
    char c;
    char pic_array[MAX_ROWS][MAX_COL];
    char only_printables[30000];

    c = *ansi_pic++;

    while (c != '\0' && count < l)
    {
        pic_array[row][col++] = c;
        count++;
        c = *ansi_pic++;
        if (c == '\n')
        {
            pic_array[row++][col] = '\0';
            maxcol = (col > maxcol) ? col : maxcol;
            col = 0;
            count++;
            c = *ansi_pic++;
        }
    }
    // Reset the pointer !
    ansi_pic -= count + 1;

    printf("\033[2J");
    printf("\033[H");
    printf("\nThe picture has total %d chars, %d rows and a maximum of %d chars per row.\n(characters + escape sequences)\n\n", count, row, maxcol);
    printf("\033[0;85H");

    for (int r = 0; r < row; r++)
    {
        for (int i = 0; i < maxcol; i++)
        {
            printf("%c", pic_array[r][i]);
            fflush(stdout);
            usleep(50);
            if (isprint(pic_array[r][i]) && pic_array[r][i] != '\n')
            {
                char t = pic_array[r][i];
                if ((t >= 65 && t <= 90) || (t >= 97 && t <= 122) || t == ':' || t == ';' || t == '.' || t == ',' || t == '0' || t == 39)
                {
                    printable++;
                    only_printables[p++] = t;
                }
            }
        }
        only_printables[p++] = '\n';
        printf("\n");
        printf("\033[5;0HPrintable chars: %d", printable);
        printf("\033[%d;85H", (r + 2));
    }

    // Null-terminate the string
    only_printables[p] = '\0';

    printf("\n");

    // ...

    return 0;
}
