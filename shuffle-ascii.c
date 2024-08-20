#include "ascii.h"

// Just a small assembly gimmick
static inline void swap(int *a, int *b)
{
    __asm__(
        "movl (%0), %%eax;\n"
        "movl (%1), %%ebx;\n"
        "movl %%ebx, (%0);\n"
        "movl %%eax, (%1);"
        :
        : "r"(a), "r"(b)
        : "%eax", "%ebx");
}

// The shuffle algorithm
static void shuffle(int *array, int n)
{
    srand(time(NULL));
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        swap(&array[i], &array[j]);
    }
}

int main()
{
    int shuffle_index, shuffled_row, shuffled_col, row, col, count, total_pixels;
    size_t l = strlen(ansi_pic);
    char pic_array[MAXY][MAXX] = {{0}};
    char clean[(int)l];
    char c;

    count = 0;

    // Jump over the 2 ansi escape sequences at the beginning
    ansi_pic += 10;

    c = *ansi_pic;

    // Clean the string from all escape sequences
    while (c != '\0' && (count + 10) < (int)l)
    {
        if (c == '\n' && (count + 10) < (int)l)
        {
            ansi_pic++;
        }
        clean[count++] = *ansi_pic++;
        c = *ansi_pic;
    }

    clean[count + 1] = '\0'; // Null termination

    row = 0;
    col = 0;

    // count rows, columns and store clean picture in 2D array
    for (int i = 0; i < strlen(clean); i++)
    {
        if (i % 80 == 0)
        {
            row++;
            col = 0;
        }
        pic_array[row][col++] = clean[i];
    }

    // Delete screen and go to position 1, 1
    printf("\033[2J\033[1;1H");

    total_pixels = row * col;
    int shuffle_array[total_pixels];

    // Fill the shuffle array with ascending numbers.
    for (int i = 0; i < total_pixels; i++)
    {
        shuffle_array[i] = i;
    }

    // Shuffle all the numbers in the array
    shuffle(shuffle_array, total_pixels);

    // Set text mode: Green bold
    printf(YELLOW);

    // Show and delete the picture with amazing shuffle effect
    for (int p = 0; p < 2; p++)
    {
        for (int i = 0; i < total_pixels; i++)
        {
            shuffle_index = shuffle_array[i];
            shuffled_row = shuffle_index / col;
            shuffled_col = shuffle_index % col;

            printf("\033[%d;%dH", shuffled_row + 1, shuffled_col + 1);
            printf("%c", pic_array[shuffled_row][shuffled_col]);
            usleep(SPEED);
            fflush(stdout);
        }
        // Show the ASCII art for 2 seconds
        sleep(2);

        // Reset text mode
        printf(RST);

        // Delete the 2D array
        for (int r = 0; r < MAXX; r++)
        {
            for (int q = 0; q < MAXY; q++)
                pic_array[q][r] = ' ';
        }
    }

    // Delete screen and go to position 1, 1
    printf("\033[2J\033[1;1H");

    return 0;
}
