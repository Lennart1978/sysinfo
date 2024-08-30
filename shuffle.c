#include "shuffle.h"

int width, height;

struct timespec req;

// Swap a and b very fast (inline Assembly test: not necessary, C is really fast enough)
static void swap(int *a, int *b)
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

// The Fischer - Yates shuffle algorithm
static void shuffle(int *array, int n)
{
    srand((unsigned int)time(NULL));
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        swap(&array[i], &array[j]);
    }
}

int show_shuffled(char *ansi_pic, int speed, char *rgb, int is_help)
{
    // The wait time between each printed char is:  speed * 10000 nanoseconds
    req.tv_nsec = speed * NSECONDS;
    req.tv_sec = 0;
    int shuffle_index, shuffled_row, shuffled_col, row, col, total_pixels, max_length = 0, c;
    int flag = 0;

    row = col = 0;

    // Pointer to the loaded string
    char *textfile;
    textfile = ansi_pic;

    // Get 2D dimension of the text file (x = max_length, y = rows)
    for (c = *textfile; c != '\0'; max_length = col > max_length ? col : max_length)
    {
        c == '\n' ? col = 0, col++, row++ : col++;
        c = *textfile++;
    }

    // Reset pointer
    textfile = ansi_pic;

    // Create 2D text array
    char pic_array[row + 1][max_length];

    width = max_length - 1;
    height = row;

    total_pixels = height * width;

    row = col = 0;

    /*Fill shorter rows with blank spaces and replace tabs with blank space.
     *Store everything in 2D array */
    while ((c = *textfile++) != '\0')
    {
        pic_array[row][col] = (char)c;
        if (c == '\n')
        {
            if (col < max_length - 1)
            {
                for (c = ' '; col <= max_length; pic_array[row][col++] = (char)c)
                    ;
            }
            col = -1;
            row++;
        }
        else if (c == '\t')
        {
            pic_array[row][col] = ' ';
        }
        col++;
    }

    // go to position 34, 1 (under the picture)
    printf("\033[34;1H");

    int shuffle_array[total_pixels];

    // Fill the shuffle array with ascending numbers.
    for (int i = 0; i < total_pixels; i++)
    {
        shuffle_array[i] = i;
    }

    // Shuffle all the numbers in the array
    shuffle(shuffle_array, total_pixels);

    int r, g, b;

    if (!strcmp(rgb, "random")) // Print with random RGB color
    {
        srand(time(NULL));
        r = rand() % 255;
        g = rand() % 255;
        b = rand() % 255;
    }
    else if (!strcmp(rgb, "red"))
        r = 255, g = 0, b = 0;
    else if (!strcmp(rgb, "green"))
        r = 0, g = 255, b = 0;
    else if (!strcmp(rgb, "yellow"))
        r = 255, g = 255, b = 0;
    else if (!strcmp(rgb, "blue"))
        r = 0, g = 0, b = 255;
    else if (!strcmp(rgb, "magenta"))
        r = 255, g = 0, b = 255;
    else if (!strcmp(rgb, "cyan"))
        r = 0, g = 255, b = 255;
    else if (!strcmp(rgb, "white"))
        r = 255, g = 255, b = 255;
    else if (!strcmp(rgb, "black"))
        r = 0, g = 0, b = 0;
    else if (!strcmp(rgb, "orange"))
        r = 255, g = 165, b = 0;
    else if (!strcmp(rgb, "grey"))
        r = 127, g = 127, b = 127;
    else
    {
        printf("\033[38;2;%sm", rgb); // Print with given RGB values
        goto jump;
    }

    printf("\033[38;2;%d;%d;%dm", r, g, b); // Print with one of the 'standard' colors

jump:

    // Hide the cursor
    printf(INVISIBLE);

    // Show and delete the ASCII picture with amazing shuffle effect
    for (int p = 0; p < (is_help ? 1 : 2); p++)
    {
        for (int i = 0; i < total_pixels; i++)
        {
            shuffle_index = shuffle_array[i];
            shuffled_row = shuffle_index / width;
            shuffled_col = shuffle_index % width;

            // Move Cursor to shuffled position
            printf("\033[%d;%dH", (shuffled_row + 1) + 34, shuffled_col + 1);
            // Print char at shuffled position
            printf("%c", pic_array[shuffled_row][shuffled_col]);
            // Wait some nanoseconds after every char
            nanosleep(&req, NULL);
            fflush(stdout);
        }

        // Show the ASCII art for 2 seconds at first glance (with bit flag)
        ((!(flag & IS_END_FLAG)) && !is_help) ? sleep(2), flag |= IS_END_FLAG : 0;

        if (!is_help)
        {
            // Shuffle the array again for deletion effect
            shuffle(shuffle_array, total_pixels);

            // Delete the 2D array (Fill with blank spaces)
            for (int r = 0; r < height; r++)
            {
                for (int q = 0; q < width; pic_array[r][q++] = ' ')
                    ;
            }
        }
    }
    // Reset text mode
    printf(RST);

    // Delete screen and go to position 1, 1 (only if not showing help)
    !is_help ? printf(CLRJ) : printf("\033[18;1H");

    // Show the cursor again
    printf(VISIBLE);

    return EXIT_SUCCESS;
}
