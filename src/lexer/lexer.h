#include "tokenlist.h"

/**
 * @brief A structure representing the lexer.
 * 
 * The `Lexer` struct contains the source file, the current location being
 * lexed, and the line num and start of the current line for error handling.
 */
typedef struct Lexer {
    char *contents; // A string contianing the source file.
    char* linestart; // A pointer to the start of the current line in the source.
    char* cursor; // A pointer to the character being lexed.
    int linenum; // The current line number.
} Lexer;

/**
 * @brief Create an new lexer from the source file.
 * 
 * Initializes the `linestart` and `cursor` to the first character in the source
 * file, and sets the linenum to 0.
 * 
 * @param source_file A string containing the source file.
 * @returns A pointer to a new lexer.
 */
Lexer *create_lexer(char *source_file);

/**
 * @brief Returns the next character in the lexer and advances the cursor.
 * 
 * If the lexer has reached the end, returns the last character in the lexer,
 * which should be a null character if the source file was properly terminated.
 */
char next_char(Lexer *lexer);

/**
 * @brief Returns the next character in the lexer without advancing the cursor.
 */
char peek_char(Lexer *lexer);

/**
 * @brief Advances the cursor of the lexer.
 */
void advance(Lexer *lexer);