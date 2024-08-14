#include "sysinfo.h"
#include "center_text.h"
#include "selfie.h"

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
            "Free Swap"RESET":%.2f GB\n"BOLD CYAN"Number of processes:"RESET"%d\n", system_text, uptime, (double)info.totalram / GB,
            (double)(info.freeram + info.bufferram + info.sharedram) / GB, (double)info.bufferram / GB, (double)info.sharedram / GB,
            (double)info.totalswap / GB, (double)info.freeswap / GB, info.procs);

    char *centered_text = center(all_text);
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
