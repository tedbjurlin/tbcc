#include <stdio.h>
#include "../air/air.h"

/**
 * @brief Emit x64 assembly code into the outfile.
 * 
 * Generates assembly code from the `prog` and emits it into the outfile.
 * 
 * @param outfile The file handle of the output file.
 * @param prog A pointer to the program.
 * @returns `0` if success, `1` otherwise.
 */
int codegen(FILE *outfile, AIR_Program *prog);