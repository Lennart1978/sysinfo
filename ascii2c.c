#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// adjust the size if needed !
#define MAX_SIZE 1048576 // 1 MB

void escape_string(char *src, char *dest) {
    while (*src) {
        if (*src == '\n') {
            *dest++ = '\\';
            *dest++ = 'n';
        } else if (*src == '"') {
            *dest++ = '\\';
            *dest++ = '"';
        } else if (*src == '\\') {
            *dest++ = '\\';
            *dest++ = '\\';
        } else {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ascii-text> <destination.c>\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Error opening source ASCII text file");
        return 1;
    }

    char buffer[MAX_SIZE];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), input_file);
    fclose(input_file);

    if (bytes_read == 0) {
        fprintf(stderr, "Error reading source or file is empty\n");
        return 1;
    }

    if (bytes_read == sizeof(buffer)) {
        fprintf(stderr, "Warning: The entire file may not have been read\n");
    }

    buffer[bytes_read] = '\0';

    char escaped_buffer[MAX_SIZE * 2];
    escape_string(buffer, escaped_buffer);

    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        perror("Error opening destination file");
        return 1;
    }

    fprintf(output_file, "const char *ansi_pic = \"%s\";\n", escaped_buffer);
    fclose(output_file);

    printf("Conversion completed. Output written in %s.\n", argv[2]);

    return 0;
}
