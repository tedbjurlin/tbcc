#include "ast.h"
#include "../lexer/tokenlist.h"

/**
* @brief Parses a list of tokens into a program.
* 
* @param tokens A pointer to a list of tokens to parse.
* @returns A pointer to the abstract syntax tree produced by the parser.
*/
AST_Program *parse(TokenList *tokens);