#ifndef TOKENLIST
#define TOKENLIST

#include "../util/strwindow.h"

/**
 * An enum representing the type of a token.
 */
typedef enum TokenType {
    T_IDENTIFIER, // Identifier.
    // constants
    T_INTCONST, // Integer constant.
    // Keywords
    T_INT, // `int` keyword.
    T_RETURN, // `return` keyword.
    T_VOID, // void keyword.
    // Symbols
    T_OCURLY, // `{` symbol.
    T_CCURLY, // `}` symbol.
    T_OPAREN, // `(` symbol.
    T_CPAREN, // `)` symbol.
    T_SEMICOLON, // `;` symbol.
} TokenType;

/**
 * @brief Struct representing a token.
 * 
 * Stores the type of the token, an optional `StrWindow` to value in the source
 * file, and information for pretty printing errors.
 */
typedef struct Token {
    char *linestart; // The start of the source line for error printing.
    StrWindow str; // An optional window to the relavent value in the source file (constant, identifier, keyword).
    int line; // The line in the source file for error printing.
    int column; // The column in the source file for error printing.
    TokenType type; // The type of the token.
} Token;

/**
 * @brief A dynamically sized list of tokens.
 */
typedef struct TokenList {
    Token *tokens; // The array of tokens.
    int cursor; // The location in the array of tokens for iterating.
    int capacity; // The capacity of the array.
    int size; // The number of tokens in the array.
} TokenList;

/**
 * @brief Creates an empty list of tokens.
 * 
 * @returns A pointer to the new token list.
 */
TokenList *create_tokenlist();

/**
 * @brief Inserts a token into the given list.
 * 
 * @param token The token to be inserted.
 * @param tokens A pointer to the token list.
 */
void insert_token(Token token, TokenList *tokens);

/**
 * @brief Returns the next token and advances the cursor.
 */
Token *next_token(TokenList *tokens);

/**
 * @brief Returns the next token without advancing the cursor.
 */
Token *peek_token(TokenList *tokens);

/**
 * @brief Advances the cursor of the token list.
 */
void advance_token(TokenList *tokens);

/**
 * @brief Frees the memory of a token list.
 */
void free_tokenlist(TokenList *tokens);

/**
 * @brief Pretty prints a token list.
 */
void pretty_print_tokenlist(TokenList *tokens);

/**
 * Formats a token for an error message.
 */
void format_token(char *out, Token *tok);

/**
 * Formats a token type for an error message.
 */
void format_token_type(char *out, TokenType type);

#endif