#ifndef PRETTYERROR
#define PRETTYERROR

/**
 * @brief Pretty prints an error.
 * 
 * The error message is printed with the line number and column, then the line 
 * with the error is printed with a marker on where the error was detected.
 * @param source_line The line the source file where the error occured.
 * @param line The line number where the error occured.
 * @param column The column number where the error occured.
 * @param message A format string containing the error message.
 * @param ... The arguments to the format string.
 */
void prettyerror(char *source_line, int line, int column, char *message, ...);

#endif