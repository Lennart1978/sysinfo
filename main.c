#define _GNU_SOURCE
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
#include "selfie.c"

#define KB      1024
#define MB      (KB * KB)
#define GB      (MB * KB)
#define MINUTE  60
#define HOUR    (MINUTE * MINUTE)
#define DAY     (HOUR * 24)
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define CYAN    "\x1b[36m"
#define BOLD    "\x1b[1m"


void printSystemInfo() {
    char hostname[256];
    struct utsname unameData;

    // Hostname
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf(BOLD CYAN "Hostname" RESET ":%s\n", hostname);
    } else {
        perror("gethostname");
    }

    // Kernel and Architecture
    if (uname(&unameData) == 0) {
        printf(BOLD CYAN "Kernel" RESET ":%s\n", unameData.release);
        printf(BOLD CYAN "Architecture" RESET ":%s\n", unameData.machine);
    } else {
        perror("uname");
    }

    // OS
    FILE *osRelease = fopen("/etc/os-release", "r");
    if (osRelease != NULL) {
        char line[256];
        char c;
        while (fgets(line, sizeof(line), osRelease)) {
            if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
                for(int i = 12; i < sizeof(line); i++) // Remove the quotes
                {
                    if(line[i] == '"')
                    {
                        line[i] = ' ';
                    }
                }
                printf(BOLD CYAN "OS" RESET ":%s", line + 13); // Skip "PRETTY_NAME="
                break;
            }
        }
        fclose(osRelease);
    } else {
        perror("fopen /etc/os-release");
    }

    // Desktop Environment
    char *desktopEnv = getenv("XDG_CURRENT_DESKTOP"); 
    if (desktopEnv != NULL) {
        printf(BOLD CYAN "Desktop Environment" RESET ":%s\n", desktopEnv);
    } else {
        printf(BOLD CYAN "Desktop Environment" RESET ":Unknown\n");
    }
    
    // CPU
    FILE *cpuInfo = fopen("/proc/cpuinfo", "r");
    if (cpuInfo != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), cpuInfo)) {
            if (strncmp(line, "model name", 10) == 0) {
                printf(BOLD CYAN "CPU" RESET ":%s", line + 13); // Skip "model name	: "
                break;
            }
        }
        fclose(cpuInfo);
    } else {
        perror("fopen /proc/cpuinfo");
    }   

    // Local IP
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == 0) {
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL)
                continue;
            if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "lo") != 0) {
                printf(BOLD CYAN "Local IP" RESET ": %s\n", inet_ntoa(((struct sockaddr_in *)ifa->ifa_addr)->sin_addr));
                break;
            }
        }
        freeifaddrs(ifaddr);
    } else {
        perror("getifaddrs");
    }
}

char* get_uptime(long uptime_seconds)
{
    char* uptime_str = malloc(100);
    uptime_str == NULL ? NULL : 0;
    
    if (uptime_seconds > DAY)
    {
        sprintf(uptime_str, "%.2f days", (double)uptime_seconds / DAY);
    }
    else if (uptime_seconds > HOUR)
    {
        sprintf(uptime_str, "%.2f hours", (double)uptime_seconds / HOUR);
    }
    else if (uptime_seconds > MINUTE)
    {
        sprintf(uptime_str, "%.2f minutes", (double)uptime_seconds / MINUTE);
    }
    else
    {
        sprintf(uptime_str, "%ld s", uptime_seconds);
    }
    
    return uptime_str;
}

int main() {
    struct sysinfo info;

    printf(ansi_pic);

    printf(BOLD GREEN"\n");
    printf("==================\n");
    printf("System Information\n");
    printf("==================\n");

    printf(RESET);

    printSystemInfo();

    (sysinfo(&info) != 0) ? (perror("sysinfo"), 1) : 0;
    
    char *uptime = get_uptime(info.uptime);

    printf(BOLD CYAN "Uptime"RESET":%s\n", uptime);
    printf(BOLD CYAN "Total RAM"RESET":%.2f GiB\n", (double)info.totalram / GB);
    printf(BOLD CYAN "Free RAM"RESET":%.2f GiB\n", (double)(info.freeram / GB));
    printf(BOLD CYAN "Shared RAM"RESET":%.2f GiB\n", (double)info.sharedram / GB);
    printf(BOLD CYAN "Buffered RAM"RESET":%.2f GiB\n", (double)info.bufferram / GB);
    printf(BOLD CYAN "Total Swap"RESET":%.2f GiB\n", (double)info.totalswap / GB);
    printf(BOLD CYAN "Free Swap"RESET":%.2f GiB\n", (double)info.freeswap / GB);
    printf(BOLD CYAN "Number of processes:"RESET"%d\n", info.procs);    

    free(uptime);

    return 0;
}