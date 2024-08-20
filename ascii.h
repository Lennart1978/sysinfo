#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

// The lower the faster is the shuffle effect
#define SPEED 2000

// Maximum picture size of 'ansi_pic' in 'ascii.c'
#define MAXX 100
#define MAXY 100

// Yellow, bold text
#define YELLOW "\033[1;3;33m"

// Reset text mode
#define RST "\033[0m"

extern const char* ansi_pic;
