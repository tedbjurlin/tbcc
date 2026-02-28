#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_ARGPARSE_IMPLEMENTATION
#include "util/stb_argparse.h"

#include "util/strwindow.h"
#include "util/readfile.h"
#include "lexer/lex.h"
#include "lexer/tokenlist.h"
#include "parser/ast.h"
#include "parser/parse.h"
#include "air/air.h"
#include "air/aerate.h"
#include "codegen/codegen.h"

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
    bool verbose = false;
    bool stop_after_lex = false;
    bool stop_after_parse = false;
    bool stop_after_codegen = false;
    const char *c_filename;

    argument_parser_t argparser;
    argparse_init(&argparser, argc, argv, "A C compiler", NO_EPILOG);

    argparse_arg_t args[] = {
        ARGPARSE_FLAG_TRUE('v', "--verbose", &verbose, "print verbose messages"),
        ARGPARSE_FLAG_TRUE('l', "--lex", &stop_after_lex, "terminate execution after lexing"),
        ARGPARSE_FLAG_TRUE('p', "--parse", &stop_after_parse, "terminate execution after parsing"),
        ARGPARSE_FLAG_TRUE('c', "--codegen", &stop_after_codegen, "terminate execution after codegen"),
        ARGPARSE_POSITIONAL(STRING, "file", &c_filename, "name of input file")
    };
    argparse_error_t aerr = argparse_add_arguments(&argparser, args, 5);
    argparse_check_error(aerr);

    // process command line arguments.
    if (argc < 2) {
        argparse_print_help(&argparser);
        return EXIT_SUCCESS;
    } else {
        argparse_error_t err = argparse_parse_args(&argparser);
        if (argparse_check_error(err)) {
            return EXIT_FAILURE;
        }
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
    char *command = malloc(15 + strlen(c_filename) + strlen(preprocessed_filename));
    sprintf(command, "gcc -E -P %s -o %s", c_filename, preprocessed_filename);

    // spawn subprocess running gcc for preprocessing
    FILE* preprocess = popen(command, "r");
    if (preprocess == NULL) {
        perror("Preprocessing failed");
        free(preprocessed_filename);
        free(command);
        return EXIT_FAILURE;
    }

    // close subprocess and check result
    int preprocess_status = pclose(preprocess);
    if (preprocess_status == -1) {
        perror("pclose failed");
        free(preprocessed_filename);
        free(command);
        return EXIT_FAILURE;
    }
    if (preprocess_status != 0) {
        fprintf(stderr, "Preprocessing failed with exit code: %d\n", preprocess_status);
        free(preprocessed_filename);
        free(command);
        return EXIT_FAILURE;
    }

    free(command);
    command = NULL;

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

    if (verbose) {
        // pretty print the tokens for debugging
        pretty_print_tokenlist(tokenlist);
    }

    // if we are targeting lexing, terminate here
    if (stop_after_lex) {
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

    if (verbose) {
        // pretty print the ast for debugging
        pretty_print_ast(ast);
    }

    // if the target is parsing, terminate here
    if (stop_after_parse) {
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
        return EXIT_FAILURE;
    }

    if (verbose) {
        // pretty print the air for debugging
        pretty_print_air(air);
    }

    if (stop_after_codegen) {
        free_air(air);
        free(source_file);
        return EXIT_SUCCESS;
    }

    // create name of .s file for listing assembly
    char *assembly_filename = malloc(strlen(c_filename));
    strcpy(assembly_filename, c_filename);
    *(assembly_filename + (filename_ext - c_filename)) = 's';

    FILE *assembly_file = fopen(assembly_filename, "w");
    if (!assembly_file) {
        perror("accessing assembly file failed");
        free_air(air);
        free(source_file);
        free(assembly_filename);
        return EXIT_FAILURE;
    }

    // generate assembly code
    if (codegen(assembly_file, air)) {
        fclose(assembly_file);
        free_air(air);
        free(source_file);
        free(assembly_filename);
        return EXIT_FAILURE;
    }

    fclose(assembly_file);

    // We don't need the AIR anymore
    free_air(air);
    air = NULL;

    // we don't need the source file any more
    free(source_file);
    source_file = NULL;

    // create filename of output binary
    char *output_filename = malloc(strlen(c_filename));
    strcpy(output_filename, c_filename);
    *(output_filename + (filename_ext - c_filename) - 1) = '\0';

    command = malloc(9 + strlen(assembly_filename) + strlen(output_filename));
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
        free(command);
        return EXIT_FAILURE;
    }

    // close assembler subprocess and process results
    int assembly_status = pclose(assemble);
    if (assembly_status == -1) {
        perror("pclose failed");
        free(command);
        return EXIT_FAILURE;
    }

    if (assembly_status != 0) {
        fprintf(stderr, "Assembling failed with exit code: %d\n", assembly_status);
        free(command);
        return EXIT_FAILURE;
    }
    free(command);

    return EXIT_SUCCESS;
}