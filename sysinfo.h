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

// Speed of the shuffle effect. The smaller the faster.
#define SPEED 50000

// This is enough for my personal picture.
#define MAX_ROWS 100
#define MAX_COL 3000

enum Desktop
{
    GNOME,
    KDE,
    XFCE
};

void swap(int *a, int *b);
void shuffle(int *array, int n);
void replace_char(char *str, char oldChar, char newChar);
void print_ansi_pic(const char *ansi_pic);
char *get_de_version(enum Desktop de);
char *get_system_info();
char *get_uptime(long uptime_seconds);
