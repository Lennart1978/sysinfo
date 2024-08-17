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
    int maxcol = 0, row = 0, col = 0, count = 0;
    char c;
    char pic_array[MAX_ROWS][MAX_COL];
    char *only_printables = malloc(l + 1);
    if (!only_printables)
    {
        fprintf(stderr, "Can't allocate memory for 'only_printables'");
        return 1;
    }

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
        }
        printf("\n");
        printf("\033[%d;85H", (r + 2));
    }

    printf("\n");

    int i = 0, j = 0;
    while (i < l)
    {
        if (ansi_pic[i] == '\033')
        {
            i++;
            while (i < l && ansi_pic[i] != 'm')
            {
                i++;
            }
            i++;
        }
        else
        {
            only_printables[j++] = ansi_pic[i++];
        }
    }

    only_printables[j] = '\0';
    size_t l2 = strlen(only_printables);
    printf("\033[5;0H");
    printf("\033[1;35mTotal printable chars: %d\n\033[0m", (int)l2);
    printf("%s", only_printables);

    return 0;
}
