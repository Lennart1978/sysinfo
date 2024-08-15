#include "sysinfo.h"

enum Desktop desktop;

char desktop_environment[100];

char *get_de_version(enum Desktop de)
{
    char *buffer = malloc(100);
    FILE *pipe = NULL;
    if (buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for buffer\n");
        return NULL;
    }
    switch (de)
    {
    case GNOME:
        pipe = popen("gnome-shell --version", "r");
        if (!pipe)
        {
            fprintf(stderr, "Failed to run gnome-shell command\n");
            return NULL;
        }
        break;

    case KDE:
        pipe = popen("plasmashell --version", "r");
        if (!pipe)
        {
            fprintf(stderr, "Failed to run plasmashell command\n");
            return NULL;
        }
        break;

    case XFCE:
        pipe = popen("xfce4-session --version", "r");
        if (!pipe)
        {
            fprintf(stderr, "Failed to run xfce4-session command\n");
            return NULL;
        }

    default:
        break;
    }

    if (fgets(buffer, 100, pipe) != NULL)
    {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
        }
    }
    else
    {
        fprintf(stderr, "Failed to read desktop environment version\n");
        free(buffer);
        return NULL;
    }
    const char *delim = " ";
    char *token;
    switch (de)
    {
    case GNOME: // Skip "GNOME Shell"
        token = strtok(buffer, delim);
        token = strtok(NULL, delim);
        token = strtok(NULL, delim);
        strcpy(buffer, token);
        buffer[strlen(buffer)] = '\0';
        break;

    case KDE: // Skip "plasmashell" - I am not shure about this !!!
        token = strtok(buffer, delim);
        token = strtok(NULL, delim);
        strcpy(buffer, token);
        buffer[strlen(buffer)] = '\0';
        break;

    case XFCE: // Skip "xfce4-session" - I am not shure abou this !!!
        token = strtok(buffer, delim);
        token = strtok(NULL, delim);
        strcpy(buffer, token);
        buffer[strlen(buffer)] = '\0';
        break;

    default:
        break;
    }

    pclose(pipe);
    return buffer;
}

char *get_system_info()
{
    char *systeminfo = malloc(100);
    if (systeminfo == NULL)
    {
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
    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        perror("gethostname");
        strcpy(hostname, "Unknown");
    }

    // Kernel and Architecture
    if (uname(&unameData) != 0)
    {
        perror("uname");
        strcpy(kernel, "Unknown");
    }
    else
    {
        snprintf(kernel, sizeof(kernel), "%s %s", unameData.sysname, unameData.release);
    }

    // OS
    FILE *osRelease = fopen("/etc/os-release", "r");
    if (osRelease != NULL)
    {
        char line[256];
        while (fgets(line, sizeof(line), osRelease))
        {
            if (strncmp(line, "PRETTY_NAME=", 12) == 0)
            {
                size_t len = strlen(line);
                if (line[len - 1] == '\n')
                {
                    line[len - 1] = '\0';
                }
                for (int i = 0; i < len; i++)
                {
                    if (line[i] == '"')
                    {
                        line[i] = ' ';
                    }
                }
                snprintf(os, sizeof(os), "%s", line + 13); // Skip "PRETTY_NAME="
                break;
            }
        }
        fclose(osRelease);
    }
    else
    {
        perror("fopen /etc/os-release");
        strcpy(os, "Unknown");
    }

    // Desktop Environment: I only implemented GNOME, KDE and XFCE until now...
    char *DE = getenv("XDG_CURRENT_DESKTOP");
    char *de_version = malloc(sizeof(float));
    if (de_version == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for 'de_version'\n");
        free(systeminfo);
    }
    if (strcmp(DE, "GNOME") == 0)
    {
        de_version = get_de_version(GNOME);
    }
    else if (strcmp(DE, "KDE") == 0)
    {
        de_version = get_de_version(KDE);
    }
    else if (strcmp(DE, "XFCE") == 0)
    {
        de_version = get_de_version(XFCE);
    }
    else
    {
        de_version = "unknown";
    }

    if (de_version == NULL)
    {
        fprintf(stderr, "Failed to get desktop environment version or unknown d.e.\n");
        free(de_version);
        free(systeminfo);
    }

    strcpy(desktop_env, DE);
    if (strlen(desktop_env) > 1)
    {
        snprintf(desktop_environment, 100, "%s %s", desktop_env, de_version);
        desktop_env[(int)(strlen(desktop_env)) - 1] = '\0';
    }
    else
    {
        strcpy(desktop_environment, "Unknown");
    }
    free(de_version);

    // CPU
    FILE *cpuInfo = fopen("/proc/cpuinfo", "r");
    if (cpuInfo != NULL)
    {
        char line[256];
        while (fgets(line, sizeof(line), cpuInfo))
        {
            if (strncmp(line, "model name", 10) == 0)
            {
                snprintf(CPU, sizeof(CPU), "%s", line + 13); // Skip "model name	: "
                size_t len = strlen(CPU);
                if (CPU[len - 1] == '\n')
                {
                    CPU[len - 1] = '\0';
                }
                break;
            }
        }
        fclose(cpuInfo);
    }
    else
    {
        perror("fopen /proc/cpuinfo");
        strcpy(CPU, "Unknown");
    }

    // Local IP
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == 0)
    {
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr == NULL)
                continue;
            if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "lo") != 0)
            {
                snprintf(local_IP, sizeof(local_IP), "%s", inet_ntoa(((struct sockaddr_in *)ifa->ifa_addr)->sin_addr));
                break;
            }
        }
        freeifaddrs(ifaddr);
    }
    else
    {
        perror("getifaddrs");
        strcpy(local_IP, "Unknown");
    }

    // Combine all the information into a single string
    snprintf(systeminfo, 4096, BOLD CYAN "Hostname:" RESET " %s\n" BOLD CYAN "Kernel:" RESET " %s\n" BOLD CYAN "OS:" RESET " %s\n" BOLD CYAN "Desktop Environment:" RESET " %s\n" BOLD CYAN "CPU:" RESET " %s\n" BOLD CYAN "Local IP:" RESET " %s\n",
             hostname, kernel, os, desktop_environment, CPU, local_IP);

    return systeminfo;
}

//  Function to get uptime in a human-readable format
char *get_uptime(long uptime_seconds)
{
    char *uptime_str = malloc(100);
    if (!uptime_str)
    {
        fprintf(stderr, "Can't allocate memory for uptime_str.\n");
    }

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
