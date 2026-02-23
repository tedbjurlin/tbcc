#include <stdio.h>
#include <stdlib.h>
#include "readfile.h"

char *readfile(char *filename) {

    FILE *input_file = fopen(filename, "r");

    if (!input_file) {
        perror("opening file failed");
        return NULL;
    }

    if (fseek(input_file, 0L, SEEK_END) != 0) {
        perror("fseek failed");
        return NULL;
    }
    long length = ftell(input_file);
    if (length == -1L) {
        perror("ftell failed");
        return NULL;
    }
    if (fseek(input_file, 0L, SEEK_SET) != 0) {
        perror("fseek failed");
        return NULL;
    }
    char *file_buff = malloc(length + 1);
    if (!file_buff) {
        perror("Failed to malloc file buffer");
        return NULL;
    }
    fread(file_buff, 1, length, input_file);
    *(file_buff + length) = '\0';

    fclose(input_file);

    return file_buff;
}