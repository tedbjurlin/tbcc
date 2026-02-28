#include "assembly_ast.h"
#include "../parser/ast.h"

/**
* Processes a C Abstract Syntax Tree (AST) to create an Assembly Intermediate
* Representation (ASM_AST). Upon failure, prints a relevant error to stderr and returns
* `NULL`.
* @param prog A pointer to the root node of the AST.
* @returns A pointer to the root node of the ASM_AST on success. `NULL` on failure.
*/
ASM_AST_Program *codegen(AST_Program *prog);