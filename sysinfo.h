#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define KB 1024
#define MB (KB * KB)
#define GB (MB * KB)
#define MINUTE 60
#define HOUR (MINUTE * 60)
#define DAY (HOUR * 24)
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define CYAN "\x1b[36m"
#define BOLD "\x1b[1m"

// This is enough for my personal picture.
#define MAX_ROWS 100
#define MAX_COL 3000

/* The lower the faster the shuffle effect
 * 2000 micro seconds = 2000000 nano seconds */
#define SPEED 80000000L



enum Desktop
{
    GNOME,
    KDE,
    XFCE
};

// Some inline Assembly experiments
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

void shuffle(int *array, int n);
void replace_char(char *str, char oldChar, char newChar);
void print_ansi_pic(const char *ansi_pic);
char *get_de_version(enum Desktop de);
char *get_system_info();
char *get_uptime(long uptime_seconds);
