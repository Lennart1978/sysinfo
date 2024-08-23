#include "sysinfo.h"

enum Desktop desktop;

char desktop_environment[100];

void print_ansi_pic(const char *ansi_pic)
{
    size_t l = strlen(ansi_pic);
    int maxcol = 0, row = 0, col = 0, count = 0;
    char c;
    char pic_array[MAX_ROWS][MAX_COL];

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

    int y, w, k;
    w = 0;
    k = 3;

    // Clear the screen and jump to 1,1
    printf("\033[2J");
    printf("\033[H");

    for (int r = 0; r < row; r++)
    {
        y = shuffle_array[r] + k;
        printf("\33[%d;%dH", y, w);
        printf("%s", ptr[y - k]);
        usleep(SPEED);
        fflush(stdout);
    }
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

char *get_de_version(enum Desktop de)
{
    char *buffer = malloc(100);
    FILE *pipe = NULL;
    const char *command = NULL;

    if (buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for buffer\n");
        return NULL;
    }

    switch (de)
    {
    case GNOME:
        command = "gnome-shell --version";
        break;
    case KDE:
        command = "plasmashell --version";
        break;
    case XFCE:
        command = "xfce4-session --version";
        break;
    default:
        free(buffer);
        return NULL;
    }

    pipe = popen(command, "r");
    if (!pipe)
    {
        fprintf(stderr, "Failed to run command: %s\n", command);
        free(buffer);
        return NULL;
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
        pclose(pipe);
        return NULL;
    }

    char *token = strtok(buffer, " ");
    for (int i = 0; i < 2 && token != NULL; i++)
    {
        token = strtok(NULL, " ");
    }

    if (token != NULL)
    {
        memmove(buffer, token, strlen(token) + 1);
    }
    else
    {
        free(buffer);
        pclose(pipe);
        return NULL;
    }

    pclose(pipe);
    return buffer;
}

char *get_system_info()
{
    char *systeminfo = malloc(4096);
    if (systeminfo == NULL)
    {
        perror("malloc");
        return NULL;
    }

    char hostname[256] = "Unknown";
    char kernel[256] = "Unknown";
    char os[256] = "Unknown";
    char CPU[256] = "Unknown";
    char local_IP[256] = "Unknown";

    struct utsname unameData;

    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        perror("gethostname");
    }

    if (uname(&unameData) == 0)
    {
        snprintf(kernel, sizeof(kernel), "%s %s", unameData.sysname, unameData.release);
    }
    else
    {
        perror("uname");
    }

    FILE *osRelease = fopen("/etc/os-release", "r");
    if (osRelease != NULL)
    {
        char line[256];
        while (fgets(line, sizeof(line), osRelease))
        {
            if (strncmp(line, "PRETTY_NAME=", 12) == 0)
            {
                char *start = strchr(line, '"');
                char *end = strrchr(line, '"');
                if (start && end && start != end)
                {
                    *end = '\0';
                    snprintf(os, sizeof(os), "%s", start + 1);
                }
                break;
            }
        }
        fclose(osRelease);
    }
    else
    {
        perror("fopen /etc/os-release");
    }

    char *DE = getenv("XDG_CURRENT_DESKTOP");
    char *de_version = NULL;
    enum Desktop de_enum;

    if (DE != NULL)
    {
        if (strcmp(DE, "GNOME") == 0)
            de_enum = GNOME;
        else if (strcmp(DE, "KDE") == 0)
            de_enum = KDE;
        else if (strcmp(DE, "XFCE") == 0)
            de_enum = XFCE;
        else
            de_enum = -1;

        if ((int)de_enum != -1)
        {
            de_version = get_de_version(de_enum);
            if (de_version != NULL)
            {
                snprintf(desktop_environment, sizeof(desktop_environment), "%s %s", DE, de_version);
                free(de_version);
            }
            else
            {
                snprintf(desktop_environment, sizeof(desktop_environment), "%s", DE);
            }
        }
        else
        {
            snprintf(desktop_environment, sizeof(desktop_environment), "%s", DE);
        }
    }

    FILE *cpuInfo = fopen("/proc/cpuinfo", "r");
    if (cpuInfo != NULL)
    {
        char line[256];
        while (fgets(line, sizeof(line), cpuInfo))
        {
            if (strncmp(line, "model name", 10) == 0)
            {
                char *start = strchr(line, ':');
                if (start)
                {
                    start++;
                    while (*start == ' ' || *start == '\t')
                        start++;
                    char *end = strchr(start, '\n');
                    if (end)
                        *end = '\0';
                    snprintf(CPU, sizeof(CPU), "%s", start);
                }
                break;
            }
        }
        fclose(cpuInfo);
    }
    else
    {
        perror("fopen /proc/cpuinfo");
    }

    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == 0)
    {
        for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "lo") != 0)
            {
                struct sockaddr_in *pAddr = (struct sockaddr_in *)ifa->ifa_addr;
                inet_ntop(AF_INET, &pAddr->sin_addr, local_IP, sizeof(local_IP));
                break;
            }
        }
        freeifaddrs(ifaddr);
    }
    else
    {
        perror("getifaddrs");
    }

    snprintf(systeminfo, 4096, BOLD CYAN "Hostname:" RESET " %s\n" BOLD CYAN "Kernel:" RESET " %s\n" BOLD CYAN "OS:" RESET " %s\n" BOLD CYAN "Desktop Environment:" RESET " %s\n" BOLD CYAN "CPU:" RESET " %s\n" BOLD CYAN "Local IP:" RESET " %s\n",
             hostname, kernel, os, desktop_environment, CPU, local_IP);

    return systeminfo;
}

char *get_uptime(long uptime_seconds)
{
    char *uptime_str = malloc(100);
    if (!uptime_str)
    {
        fprintf(stderr, "Can't allocate memory for uptime_str.\n");
        return NULL;
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
