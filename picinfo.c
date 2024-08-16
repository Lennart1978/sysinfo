#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

    int maxcol, row, col, count = 0;
    char c;
    char pic_array[MAX_ROWS][MAX_COL];

    c = ansi_pic[count];

    while (c != '\0' && count < l)
    {
        pic_array[row][col] = c;
        col++;
        count++;
        c = ansi_pic[count];
        if (c == '\n')
        {
            pic_array[row][col] = '\0';
            row++;
            if (col > maxcol)
                maxcol = col;
            col = 0;
            count++;
            c = ansi_pic[count];
        }
    }

    printf("\033[2J");
    printf("\033[H");
    printf("\nThe picture has total %d chars, %d rows and a maximum of %d chars per row.\n(characters + escape sequences)\n\n", count, row, maxcol);
    printf("\033[0;85H");

    for (int r = 0; r < row; r++)
    {
        for (int i = 0; i < maxcol; i++)
        {
            printf("%c", pic_array[r][i]);
            usleep(50); // Only for the print effect :-)
            fflush(stdout);
        }
        printf("\n");
        printf("\033[%d;85H", (r + 2));
    }

    printf("\n");

    return 0;
}
