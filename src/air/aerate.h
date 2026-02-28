#include "air.h"
#include "../parser/ast.h"

/**
* Processes a C Abstract Syntax Tree (AST) to create an Assembly Intermediate
* Representation (AIR). Upon failure, prints a relevant error to stderr and returns
* `NULL`.
* @param prog A pointer to the root node of the AST.
* @returns A pointer to the root node of the AIR on success. `NULL` on failure.
*/
AIR_Program *aerate(AST_Program *prog);