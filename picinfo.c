#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include "selfie.h"

// This is enough for my personal picture.
#define MAX_ROWS 100
#define MAX_COL 3000

// The time to wait between the rows
#define SPEED 80000

/*This little program counts the chars(plus escape sequences), -
 *rows and the maximum of chars per row of the picture in 'ansi_pic'.
 *It then stores the picture in a 2D array and prints it out on the right side of the information.
 *It counts the printable chars and prints them out as picture.
 *The pictures are displayed with a random - row effect.
 */

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Fisher-Yates Shuffle Algorithm
void shuffle(int *array, int n)
{
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        swap(&array[i], &array[j]);
    }
}

void replace_char(char *str, char oldChar, char newChar)
{
    while (*str)
    {
        if (*str == oldChar)
        {
            *str = newChar;
        }
        str++;
    }
}

int main(void)
{
    srand(time(NULL));
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

    // Store the ansi pic in a 2D array
    while (c != '\0' && count < (int)l)
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

    // Pointer to the beginning of each row
    char *ptr[row];

    // Replace '\n' with '\0' in the pic_array strings
    for (int r = 0; r < row; r++)
    {
        ptr[r] = &pic_array[r][0];
        replace_char(ptr[r], '\n', '\0');
    }

    int shuffle_array[row];

    // Fill the shuffle array
    for (int i = 0; i < row; i++)
    {
        shuffle_array[i] = i;
    }

    // Shuffle the shuffle array
    shuffle(shuffle_array, row);

    printf("\033[0;85H");

    int y, w, k;
    w = 85;
    k = 0;

    // Print the picture 2 times with random - shuffle effect
    for (int z = 0; z <= 1; z++)
    {
        for (int r = 0; r < row; r++)
        {
            y = shuffle_array[r] + k;
            printf("\33[%d;%dH", y, w);
            printf("%s", ptr[y - k]);
            usleep(SPEED);
            fflush(stdout);
        }
        w = 0;
        k = 6;
    }

    int i = 0, j = 0, countm = 0;

    // Count the printable chars
    while (i < (int)l)
    {
        if (ansi_pic[i] == '\033')
        {
            i++;
            while (i < (int)l && ansi_pic[i] != 'm')
            {
                i++;
            }
            i++;
        }
        else
        {
            countm++;
            if (countm == 2) // After the third 'm' comes the printable char (countm starts with 0)
            {
                only_printables[j++] = ansi_pic[i++];
                countm = 0;
            }
        }
    }

    // Null terminate the string
    only_printables[j] = '\0';

    size_t l2 = strlen(only_printables);

    printf("\033[5;0H");
    printf("\033[1;35mTotal printable chars: %d\n\033[0m", (int)l2);
    printf("%s", only_printables);

    // Don't forget to free the memory !
    free(only_printables);

    return 0;
}
