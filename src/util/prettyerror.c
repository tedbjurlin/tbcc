#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "prettyerror.h"

// prints error message and indicates location in file.
// appends newline onto message
// source line is a pointer to the start of the line
void prettyerror(char *source_line, int line, int column, char *message, ...) {
  va_list arguments;
  va_start(arguments, message);
  
  fprintf(stderr, "%d:%d: error: ", line, column);
  vprintf(message, arguments);
  puts("");
  
  va_end(arguments); // Cleans up the list
  
  char f_line[128];
  int n = sscanf(source_line, "%127[^\n]", f_line);
  
  fprintf(stderr, "%5d | %s\n", line, f_line);
  fprintf(stderr, "%5s | %*s\n", "", column, "^");
}