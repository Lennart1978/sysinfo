#include "center_text.h"

// Function to center text
char *center(const char *text) {
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
