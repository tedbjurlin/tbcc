#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tokenlist.h"
#include "lexer.h"
#include "lex.h"
#include "../util/prettyerror.h"

Token simple_token(TokenType type, char *linestart, int line, int column) {
    return (Token){
        .type = type,
        .str = create_str_window(NULL, 0),
        .linestart = linestart,
        .line = line,
        .column = column,
    };
}

Token lex_number(Lexer *lexer) {
    char next = peek_char(lexer);
    char *window_start = lexer->cursor - 1;
    int window_size = 1;

    while (isdigit(next)) {
        ++window_size;
        advance(lexer);
        next = peek_char(lexer);
    }

    return (Token) {
        .type = T_INTCONST,
        .str = create_str_window(window_start, window_size),
        .linestart = lexer->linestart,
        .line = lexer->linenum,
        .column = lexer->cursor - lexer->linestart
    };
}

Token lex_identifier(Lexer *lexer) {
    char next = peek_char(lexer);
    char *window_start = lexer->cursor - 1;
    int window_size = 1;

    while (isalnum(next) || next == '_') {
        ++window_size;
        advance(lexer);
        next = peek_char(lexer);
    }

    return (Token) {
        .type = T_IDENTIFIER,
        .str = create_str_window(window_start, window_size),
        .linestart = lexer->linestart,
        .line = lexer->linenum,
        .column = lexer->cursor - lexer->linestart
    };
}

Token lex_keyword_if(Token token) {
    if (strncmp("int", token.str.ptr, token.str.size) == 0) {
        token.type = T_INT;
    } else if (strncmp("return", token.str.ptr, token.str.size) == 0) {
        token.type = T_RETURN;
    }

    return token;
}

TokenList *lex(char *source_file) {
    Lexer *lexer = create_lexer(source_file);
    if (!lexer) {
        return NULL;
    }

    TokenList *tokenlist = create_tokenlist(lexer->contents);
    if (!tokenlist) {
        return NULL;
    }

    char next = next_char(lexer);
    while (next != '\0') {
        if (isspace(next)) {
            next = next_char(lexer);
            continue;
        }

        switch (next) {
            case '{':
                insert_token(simple_token(
                    T_OCURLY,
                    lexer->linestart,
                    lexer->linenum,
                    lexer->cursor - lexer->linestart
                ), tokenlist);
                break;
            case '}':
                insert_token(simple_token(
                    T_CCURLY,
                    lexer->linestart,
                    lexer->linenum,
                    lexer->cursor - lexer->linestart
                ), tokenlist);
                break;
            case '(':
                insert_token(simple_token(
                    T_OPAREN,
                    lexer->linestart,
                    lexer->linenum,
                    lexer->cursor - lexer->linestart
                ), tokenlist);
                break;
            case ')':
                insert_token(simple_token(
                    T_CPAREN,
                    lexer->linestart,
                    lexer->linenum,
                    lexer->cursor - lexer->linestart
                ), tokenlist);
                break;
            case ';':
                insert_token(simple_token(
                    T_SEMICOLON,
                    lexer->linestart,
                    lexer->linenum,
                    lexer->cursor - lexer->linestart
                ), tokenlist);
                break;
            case '_':
                insert_token(lex_identifier(lexer), tokenlist);
            default:
                if (isalpha(next)) {
                    Token token = lex_identifier(lexer);
                    insert_token(
                        lex_keyword_if(token),
                        tokenlist
                    );
                } else if (isdigit(next)) {
                    insert_token(lex_number(lexer), tokenlist);
                } else {
                    prettyerror(
                        lexer->linestart,
                        lexer->linenum + 1,
                        lexer->cursor - lexer->linestart,
                        "lexing failed: unknown symbol '%c'",
                        next
                    );
                    free_tokenlist(tokenlist);
                    free(lexer);
                    return NULL;
                }
        }

        next = next_char(lexer);
    }

    free(lexer);

    return tokenlist;
}