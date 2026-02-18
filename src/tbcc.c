#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return NULL;
    return dot + 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: tbcc INPUT_FILE\n");
        return EXIT_FAILURE;
    }

    const char* filename_ext = get_filename_ext(argv[1]);

    if (filename_ext == NULL) {
        fprintf(stderr, "Invalid filename: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (strcmp(filename_ext, "c") != 0) {
        fprintf(stderr, "Input file must be a C source file\n");
        return EXIT_FAILURE;
    }

    char *preprocessed_filename = malloc(strlen(argv[1]));
    strcpy(preprocessed_filename, argv[1]);
    *(preprocessed_filename + (filename_ext - argv[1])) = 'i';

    char command[128];

    sprintf(command, "gcc -E -P %s -o %s", argv[1], preprocessed_filename);

    FILE* preprocess = popen(command, "r");
    if (preprocess == NULL) {
        perror("Preprocessing failed");
        free(preprocessed_filename);
        return EXIT_FAILURE;
    }

    int preprocess_status = pclose(preprocess);
    if (preprocess_status == -1) {
        perror("pclose failed");
        free(preprocessed_filename);
        return EXIT_FAILURE;
    }

    if (preprocess_status != 0) {
        fprintf(stderr, "Preprocessing failed with exit code: %d\n", preprocess_status);
        free(preprocessed_filename);
        return EXIT_FAILURE;
    }

    free(preprocessed_filename);

    char *assembly_filename = malloc(strlen(argv[1]));
    strcpy(assembly_filename, argv[1]);
    *(assembly_filename + (filename_ext - argv[1])) = 's';

    char *output_filename = malloc(strlen(argv[1]));
    strcpy(output_filename, argv[1]);
    *(output_filename + (filename_ext - argv[1]) - 1) = '\0';

    sprintf(command, "gcc %s -o %s", assembly_filename, output_filename);

    free(assembly_filename);
    free(output_filename);


    FILE* assemble = popen(command, "r");
    if (assemble == NULL) {
        perror("Assembly failed");
        free(assembly_filename);
        free(output_filename);
        return EXIT_FAILURE;
    }

    int assembly_status = pclose(assemble);
    if (assembly_status == -1) {
        perror("pclose failed");
        free(assembly_filename);
        free(output_filename);
        return EXIT_FAILURE;
    }

    if (assembly_status != 0) {
        fprintf(stderr, "Preprocessing failed with exit code: %d\n", assembly_status);
        free(assembly_filename);
        free(output_filename);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}