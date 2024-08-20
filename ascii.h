#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

// The lower the faster is the shuffle effect
#define SPEED 2000

// Maximum picture size of 'ansi_pic' in 'ascii.c'
#define MAXX 100
#define MAXY 100

// The actual width of my ASCII picture
#define WIDTH 80

// Yellow, bold text
#define YELLOW "\033[1;3;33m"

// Hide the cursor
#define INVISIBLE "\033[?25l"

// Show the cursor
#define VISIBLE "\033[?25h"

// Clear screen and jump to position 1,1
#define CLRJ "\033[2J\033[1;1H"

// Clear screen
#define CLR "\033[2J"

// Reset text mode
#define RST "\033[0m"

// The definition of 'ansi_pic' is in file 'ascii.c'
extern const char *ansi_pic;

// Just a small assembly gimmick: It swaps a and b very fast.
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
