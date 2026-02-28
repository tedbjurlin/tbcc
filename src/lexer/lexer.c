#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "lexer.h"
#include "tokenlist.h"

Lexer *create_lexer(char *source_file) {
  
  Lexer *lexer = malloc(sizeof(Lexer));
  
  lexer->contents = source_file;
  lexer->linenum = 0;
  lexer->linestart = source_file;
  lexer->cursor = source_file;
  
  return lexer;
}

char next_char(Lexer *lexer) {
  char next = *lexer->cursor;
  
  advance(lexer);
  
  return next;
}

char peek_char(Lexer *lexer) {
  return *lexer->cursor;
}


void advance(Lexer *lexer) {
  char next = *lexer->cursor;
  
  if (next != '\0') {
    lexer->cursor += 1;
    
    if (*lexer->cursor == '\r' && *(lexer->cursor + 1) == '\n') {
      *lexer->cursor = ' ';
    } else if (*lexer->cursor == '\r') {
      *lexer->cursor = '\n';
    }
    
    if (next == '\n') {
      ++lexer->linenum;
      lexer->linestart = lexer->cursor;
    }
  }
}