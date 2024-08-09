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
#define HOUR    (MINUTE * 60)
#define DAY     (HOUR * 24)
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define CYAN    "\x1b[36m"
#define BOLD    "\x1b[1m"

// I only implemented Gnome DE because I use it, but you can add more if you want.
char* get_gnome_version()
{
    char *buffer = malloc(100);
    if(buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for buffer\n");
        return NULL;
    }

    FILE* pipe = popen("gnome-shell --version", "r");
    if (!pipe) {
        fprintf(stderr, "Failed to run gnome-shell command\n");        
        return NULL;
    }    

    
    if (fgets(buffer, 100, pipe) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    } else {
        fprintf(stderr, "Failed to read gnome-shell version\n");
        free(buffer);
        return NULL;
    }
    buffer += 12; // Remove the "gnome-shell " prefix
    buffer[strlen(buffer)] = '\0';    
    
    pclose(pipe);
    return buffer;
}

char* get_system_info() {
    char *systeminfo = malloc(4096);
    if (systeminfo == NULL) {
        perror("malloc");
        return NULL;
    }

    char hostname[256];
    char kernel[256];
    char architecture[256];
    char os[256];
    char desktop_env[256];
    char CPU[256];
    char local_IP[256];

    struct utsname unameData;

    // Hostname
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname");
        strcpy(hostname, "Unknown");
    }

    // Kernel and Architecture
    if (uname(&unameData) != 0) {
        perror("uname");
        strcpy(kernel, "Unknown");
    } else {
        snprintf(kernel, sizeof(kernel), "%s %s", unameData.sysname, unameData.release);
    }

    // OS
    FILE *osRelease = fopen("/etc/os-release", "r");
    if (osRelease != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), osRelease)) {
            if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
                size_t len = strlen(line);
                if (line[len - 1] == '\n') {
                    line[len - 1] = '\0';
                }
                for(int i = 0; i < len; i++) {
                    if (line[i] == '"') {
                        line[i] = ' ';
                    }
                }
                snprintf(os, sizeof(os), "%s", line + 13); // Skip "PRETTY_NAME="
                break;
            }
        }
        fclose(osRelease);
    } else {
        perror("fopen /etc/os-release");
        strcpy(os, "Unknown");
    }

    // Desktop Environment
    char *gnome_version = get_gnome_version();
    if(gnome_version == NULL)
    {
        fprintf(stderr, "Failed to get gnome version\n");
        gnome_version -=12; // Set pointer back !
        free(gnome_version);
        free(systeminfo);
        return NULL;
    }

    char *desktopEnv = getenv("XDG_CURRENT_DESKTOP");
    if (desktopEnv != NULL) {
        snprintf(desktop_env, sizeof(desktop_env) + sizeof(gnome_version), "%s %s", desktopEnv, gnome_version);        
    } else {
        strcpy(desktop_env, "Unknown");
    }
    gnome_version -= 12; // Set pointer back !
    free(gnome_version);

    // CPU
    FILE *cpuInfo = fopen("/proc/cpuinfo", "r");
    if (cpuInfo != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), cpuInfo)) {
            if (strncmp(line, "model name", 10) == 0) {
                snprintf(CPU, sizeof(CPU), "%s", line + 13); // Skip "model name	: "
                size_t len = strlen(CPU);
                if (CPU[len - 1] == '\n') {
                    CPU[len - 1] = '\0';
                }
                break;
            }
        }
        fclose(cpuInfo);
    } else {
        perror("fopen /proc/cpuinfo");
        strcpy(CPU, "Unknown");
    }

    // Local IP
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == 0) {
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL)
                continue;
            if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "lo") != 0) {
                snprintf(local_IP, sizeof(local_IP), "%s", inet_ntoa(((struct sockaddr_in *)ifa->ifa_addr)->sin_addr));                
                break;
            }
        }
        freeifaddrs(ifaddr);
    } else {
        perror("getifaddrs");
        strcpy(local_IP, "Unknown");
    }

    // Combine all the information into a single string
    snprintf(systeminfo, 4096, BOLD CYAN "Hostname:"RESET" %s\n"BOLD CYAN "Kernel:"RESET" %s\n"BOLD CYAN"OS:"
            RESET" %s\n"BOLD CYAN"Desktop Environment:"RESET" %s\n"BOLD CYAN"CPU:"RESET" %s\n"BOLD CYAN"Local IP:"RESET" %s\n",
             hostname, kernel, os, desktop_env, CPU, local_IP);
       
    return systeminfo;
}

// Function to center text
char* center(const char *text) {
    char *result = NULL;
    char *line, *saveptr;
    char *input = strdup(text);
    int max_line_length = 0;
    int total_length = 0;
    int line_count = 0;

    line = strtok_r(input, "\n", &saveptr);
    while (line != NULL) {
        int len = strlen(line);
        if (len > max_line_length) {
            max_line_length = len;
        }
        total_length += len + 1; 
        line_count++;
        line = strtok_r(NULL, "\n", &saveptr);
    }

    result = (char*)malloc(total_length + line_count * max_line_length + 1);
    if (result == NULL) {
        free(input);
        return NULL;
    }

    char *output = result;
    const char *input_ptr = text;
    while (*input_ptr) { 
        if (*input_ptr == '\n' && (input_ptr == text || *(input_ptr - 1) == '\n')) {
            *output++ = '\n';
            input_ptr++;
            continue;
        }

        const char *end = strchr(input_ptr, '\n');
        if (!end) end = input_ptr + strlen(input_ptr);

        int line_length = end - input_ptr;
        int padding = (max_line_length - line_length) / 2;

        for (int i = 0; i < padding; i++) {
            *output++ = ' ';
        }
        strncpy(output, input_ptr, line_length);
        output += line_length;
        *output++ = '\n';

        input_ptr = *end ? end + 1 : end;
    }
    *output = '\0';

    free(input);
    return result;
}

//  Function to get uptime in a human-readable format
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

    // This is all to print the ASCII art
    printf("%s", ansi_pic);

    if (sysinfo(&info) != 0) {
        perror("sysinfo");
        return 1;
    }
    
    char *uptime = get_uptime(info.uptime);
    if (uptime == NULL) {
        fprintf(stderr, "Failed to get uptime\n");
        return 1;
    }

    char *system_text = get_system_info();
    if (system_text == NULL) {
        fprintf(stderr, "Failed to get system info\n");
        free(uptime);
        return 1;
    }

    char *all_text = malloc(8192);
    if (all_text == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(uptime);
        free(system_text);
        return 1;
    }
    

    snprintf(all_text, 8192, "%s"BOLD CYAN"Uptime"RESET":%s\n"BOLD CYAN"Total RAM"RESET":%.2f GB\n"BOLD CYAN"Free RAM"RESET":%.2f GB\n"
            BOLD CYAN "Buffered RAM"RESET":%.2f GB\n"BOLD CYAN"Shared RAM"RESET":%.2f GB\n"BOLD CYAN"Total Swap"RESET":%.2f GB\n"BOLD CYAN
            "Free Swap"RESET":%.2f GB\n"BOLD CYAN"Number of processes:"RESET"%ld\n", system_text, uptime, (double)info.totalram / GB,
            (double)(info.freeram + info.bufferram + info.sharedram) / GB, (double)info.bufferram / GB, (double)info.sharedram / GB,
            (double)info.totalswap / GB, (double)info.freeswap / GB, info.procs);

    char* centered_text = center(all_text);    
    if (centered_text == NULL) {
        fprintf(stderr, "Failed to center text\n");
        free(all_text);
        free(uptime);
        free(system_text);
        return 1;
    }

    printf("%s\n", centered_text);

    // Don't forget to free the allocated memory !
    free(centered_text);
    free(all_text);
    free(system_text);
    free(uptime);

    return 0;
}