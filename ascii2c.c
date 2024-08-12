#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Escapes special characters in a string by replacing them with their escaped counterparts.
 * 
 * This function takes a source string and a destination buffer, and escapes any occurrences of newline ('\n'),
 * double quote ('"'), and backslash ('\\') characters in the source string. The escaped string is written to
 * the destination buffer, which must be large enough to hold the escaped string (at most twice the length of
 * the source string).
 * 
 * @param src The source string to be escaped.
 * @param dest The destination buffer to write the escaped string to.
 * @param dest_size The size of the destination buffer, in bytes.
 */
void escape_string(const char *src, char *dest, size_t dest_size) {
    size_t i = 0;
    while (*src && i < dest_size - 1) {
        if (*src == '\n') {
            if (i < dest_size - 2) {
                dest[i++] = '\\';
                dest[i++] = 'n';
            } else {
                break;
            }
        } else if (*src == '"') {
            if (i < dest_size - 2) {
                dest[i++] = '\\';
                dest[i++] = '"';
            } else {
                break;
            }
        } else if (*src == '\\') {
            if (i < dest_size - 2) {
                dest[i++] = '\\';
                dest[i++] = '\\';
            } else {
                break;
            }
        } else {
            dest[i++] = *src;
        }
        src++;
    }
    dest[i] = '\0';
}

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ascii-text> <destination.c>\n", argv[0]);
        return 1;
    }

    // Open the source ASCII text file for reading
    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Error opening source ASCII text file");
        return 1;
    }

    // Get the size of the source ASCII text file
    fseek(input_file, 0, SEEK_END);
    size_t file_size = ftell(input_file);
    rewind(input_file);

    // Allocate memory for the buffer to hold the source ASCII text. Add 1 for the null terminator.
    char *buffer = malloc(file_size + 1);
    if (!buffer) {
        perror("Memory allocation error for buffer");
        fclose(input_file);
        return 1;
    }

    // Read the source ASCII text into the buffer
    size_t bytes_read = fread(buffer, 1, file_size, input_file);
    fclose(input_file);

    if (bytes_read == 0) {
        fprintf(stderr, "Error reading source or file is empty\n");
        free(buffer);
        return 1;
    }    

    // Null-terminate the buffer
    buffer[bytes_read] = '\0';

    // Allocate memory for the escaped buffer. Add 1 for the null terminator.
    char *escaped_buffer = malloc(file_size * 2 + 1);
    if (!escaped_buffer) {
        perror("Memory allocation error for escaped_buffer");
        // Free the buffer before returning !
        free(buffer);
        return 1;
    }
    
    // Escape the buffer and write the escaped string to the destination file
    escape_string(buffer, escaped_buffer, file_size * 2 + 1);

    //  Open the destination file for writing
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        perror("Error opening destination file");
        // Free the buffers before returning !
        free(buffer);
        free(escaped_buffer);
        return 1;
    }

    // Write the escaped string to the destination file and save it in a string
    int write_result = snprintf(NULL, 0, "const char *ansi_pic = \"%s\";\n", escaped_buffer);
    if (write_result > 0) {
        // Allocate memory for the output string. Add 1 for the null terminator.
        char *output_string = malloc(write_result + 1);
        if (output_string) {
            // The escaped ASCII string is written to the output string. String 'ansi_pic' holds the escaped string.
            snprintf(output_string, write_result + 1, "const char *ansi_pic = \"%s\";\n", escaped_buffer);
            // Write the output string to the destination file
            fputs(output_string, output_file);
            // Free the allocated memory for the output string !
            free(output_string);
        } else {
            perror("Memory allocation error for output string");
        }
    } else {
        fprintf(stderr, "Error formatting output string\n");
    }

    fclose(output_file);

    printf("Conversion completed. Output written in %s.\n", argv[2]);

    // Free the allocated memory for the buffers !
    free(buffer);
    free(escaped_buffer);

    return 0;
}