#include "sysinfo.h"
#include "center_text.h"
#include "selfie.h"
#include "shuffle.h"
#include <errno.h>
#include <string.h>

#define MAX_BUFFER_SIZE 8192

int main()
{
    struct sysinfo info;

    // Print the colored ansi pic with shuffle effect
    print_ansi_pic(ansi_pic);

    if (sysinfo(&info) != 0)
    {
        fprintf(stderr, "sysinfo error: %s\n", strerror(errno));
        return 1;
    }

    char *uptime = get_uptime(info.uptime);
    if (uptime == NULL)
    {
        fprintf(stderr, "Failed to get uptime\n");
        return 1;
    }

    char *system_text = get_system_info();
    if (system_text == NULL)
    {
        fprintf(stderr, "Failed to get system info\n");
        free(uptime);
        return 1;
    }

    char *all_text = malloc(MAX_BUFFER_SIZE);
    if (all_text == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        free(uptime);
        free(system_text);
        return 1;
    }

    int result = snprintf(all_text, MAX_BUFFER_SIZE - 1, "%s Uptime: %s\nTotal RAM: %.2f GB\nFree RAM: %.2f GB\nBuffered RAM: %.2f GB\nShared RAM: %.2f GB\nTotal Swap: %.2f GB\nFree Swap: %.2f GB\nNumber of processes: %lu\n", system_text, uptime, (double)info.totalram / GB,
                          (double)(info.freeram + info.bufferram + info.sharedram) / GB, (double)info.bufferram / GB, (double)info.sharedram / GB,
                          (double)info.totalswap / GB, (double)info.freeswap / GB, (unsigned long)info.procs);

    if (result < 0 || result >= MAX_BUFFER_SIZE - 1)
    {
        fprintf(stderr, "Error formatting system information\n");
        free(all_text);
        free(uptime);
        free(system_text);
        return 1;
    }

    char *centered_text = center(all_text);
    if (centered_text == NULL)
    {
        fprintf(stderr, "Failed to center text\n");
        free(all_text);
        free(uptime);
        free(system_text);
        return 1;
    }

    printf("\033[1m\033[34;1H");
    show_shuffled(centered_text, 100, "cyan", HELP);
    printf("\033[0m\033[50;1H");
    // Don't forget to free the allocated memory !
    free(centered_text);
    free(all_text);
    free(system_text);
    free(uptime);

    return 0;
}
