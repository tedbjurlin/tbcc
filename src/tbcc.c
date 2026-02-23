#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util/strwindow.h"
#include "util/readfile.h"
#include "lexer/lex.h"
#include "lexer/tokenlist.h"
#include "parser/ast.h"
#include "parser/parse.h"
#include "air/air.h"
#include "air/aerate.h"

/**
 * @brief Represents the target compilation stage represented by the user.
 */
typedef enum COMPSTAGE {
    COMPILE, // Compile completely
    LEXING, // Stop after lexing
    PARSING, // Stop after parsing
} COMPSTAGE;

/**
 * @brief Finds the start of the file extension in the file name.
 * @param filename The filename to search.
 * @returns A pointer to the start of file extension. If the filename does not
 * have an extension or the only `.` in the filename is the first character,
 * `NULL` is returned.
 */
const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return NULL;
    return dot + 1;
}

int main(int argc, char *argv[]) {
    COMPSTAGE target_stage = COMPILE;
    char *c_filename;

    // process command line arguments. I would like to find a more idomatic
    // way to do this at some point, but currently we are working fast and dirty
    if (argc == 2) {
        c_filename = argv[1];
    } else if (argc == 3) {
        if (strcmp(argv[1], "--lex") == 0) {
            target_stage = LEXING;
            c_filename = argv[2];
        } else if (strcmp(argv[1], "--parse") == 0) {
            target_stage = PARSING;
            c_filename = argv[2];
        } else if (strcmp(argv[2], "--lex") == 0) {
            target_stage = LEXING;
            c_filename = argv[1];
        } else if (strcmp(argv[2], "--parse") == 0) {
            target_stage = PARSING;
            c_filename = argv[1];
        }
    } else {
        fprintf(stderr, "Usage: tbcc [--lex|--parse] INPUT_FILE\n");
        return EXIT_FAILURE;
    }

    const char* filename_ext = get_filename_ext(c_filename);

    if (filename_ext == NULL) {
        fprintf(stderr, "Invalid filename: %s\n", c_filename);
        return EXIT_FAILURE;
    }
    if (strcmp(filename_ext, "c") != 0) {
        fprintf(stderr, "Input file must be a C source file\n");
        return EXIT_FAILURE;
    }

    // create filename for .i file from preprocessing
    char *preprocessed_filename = malloc(strlen(c_filename));
    strcpy(preprocessed_filename, c_filename);
    *(preprocessed_filename + (filename_ext - c_filename)) = 'i';

    // set up command for preprocessing
    char command[128];
    sprintf(command, "gcc -E -P %s -o %s", c_filename, preprocessed_filename);

    // spawn subprocess running gcc for preprocessing
    FILE* preprocess = popen(command, "r");
    if (preprocess == NULL) {
        perror("Preprocessing failed");
        free(preprocessed_filename);
        return EXIT_FAILURE;
    }

    // close subprocess and check result
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

    // read the preprocessed file as our source file
    //
    // this source file will be our source of truth for all strings and for
    // pretty printing errors, so it won't be freed until the end of the program
    char *source_file = readfile(preprocessed_filename);
    if (!source_file) {
        free(preprocessed_filename);
        return EXIT_FAILURE;
    }

    // lex the source file into a list of tokens
    TokenList *tokenlist = lex(source_file);
    if (!tokenlist) {
        free(source_file);
        free(preprocessed_filename);
        return EXIT_FAILURE;
    }

    // pretty print the tokens for debugging
    pretty_print_tokenlist(tokenlist);

    // if we are targeting lexing, terminate here
    if (target_stage == LEXING) {
        free(source_file);
        free(preprocessed_filename);
        free_tokenlist(tokenlist);
        return EXIT_SUCCESS;
    }

    // we don't need the name of the preprocessed fiel any more.
    free(preprocessed_filename);
    preprocessed_filename = NULL;

    // parse the list of tokens into an abstract syntax tree
    AST_Program *ast = parse(tokenlist);
    
    // we don't need the list of tokens any more.
    free_tokenlist(tokenlist);
    tokenlist = NULL;

    if (!ast) {
        free(source_file);
        return EXIT_FAILURE;
    }

    // pretty print the ast for debugging
    pretty_print_ast(ast);

    // if the target is parsing, terminate here
    if (target_stage == PARSING) {
        free(source_file);
        free_ast(ast);
        return EXIT_SUCCESS;
    }

    // process the AST into an AIR (Assembly Intermediate Representation)
    AIR_Program *air = aerate(ast);

    // we don't need the AST any more
    free_ast(ast);
    ast = NULL;

    if (!air) {
        free(source_file);
        free_air(air);
        return EXIT_SUCCESS;
    }

    // pretty print the AIR for debugging
    pretty_print_air(air);

    // create name of .s file for listing assembly
    char *assembly_filename = malloc(strlen(c_filename));
    strcpy(assembly_filename, c_filename);
    *(assembly_filename + (filename_ext - c_filename)) = 's';

    // TODO: we will eventually do codegen here

    // we don't need the source file any more
    free(source_file);
    source_file = NULL;

    // create filename of output binary
    char *output_filename = malloc(strlen(c_filename));
    strcpy(output_filename, c_filename);
    *(output_filename + (filename_ext - c_filename) - 1) = '\0';

    // create command to assemble program
    sprintf(command, "gcc %s -o %s", assembly_filename, output_filename);

    // we don't need these filenames any more
    free(assembly_filename);
    assembly_filename = NULL;
    free(output_filename);
    output_filename = NULL;

    // create subprocess to run gcc assembler
    FILE* assemble = popen(command, "r");
    if (assemble == NULL) {
        perror("Assembly failed");
        free(assembly_filename);
        free(output_filename);
        return EXIT_FAILURE;
    }

    // close assembler subprocess and process results
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