#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tokenlist.h"

#define STARTING_CAPACITY 32

TokenList *create_tokenlist() {
    TokenList *tokens = malloc(sizeof(TokenList));

    tokens->tokens = malloc(sizeof(Token) * STARTING_CAPACITY);
    tokens->cursor = 0;
    tokens->capacity = STARTING_CAPACITY;
    tokens->size = 0;
    return tokens;
}

void insert_token(Token token, TokenList *tokens) {
    if (tokens->size == tokens->capacity) {
        tokens->capacity *= 2;
        tokens->tokens = realloc(tokens->tokens, tokens->capacity);
    }
    tokens->tokens[tokens->size] = token;
    ++tokens->size;
}

Token *next_token(TokenList *tokens) {
    Token *next = peek_token(tokens);
    advance_token(tokens);
    return next;
}

Token *peek_token(TokenList *tokens) {
    return tokens->tokens + tokens->cursor;
};

void advance_token(TokenList *tokens) {
    if (tokens->cursor < tokens->size) {
        ++tokens->cursor;
    }
}

void free_tokenlist(TokenList *tokens) {
    free(tokens->tokens);
    free(tokens);
}

// Pretty printing

void pretty_print_token(Token token) {
    switch (token.type) {
        case T_INT:
            printf("T_INT");
            break;
        case T_RETURN:
            printf("T_RETURN");
            break;
        case T_OCURLY:
            printf("T_OCURLY");
            break;
        case T_CCURLY:
            printf("T_CCURLY");
            break;
        case T_OPAREN:
            printf("T_OPAREN");
            break;
        case T_CPAREN:
            printf("T_CPAREN");
            break;
        case T_SEMICOLON:
            printf("T_SEMICOLON");
            break;
        case T_IDENTIFIER:
            printf("T_IDENTIFIER");
            break;
        case T_INTCONST:
            printf("T_INTCONST");
            break;
    }
    if (token.type < 4) {
        char *p = malloc(token.str.size + 1);
        snprintf(p, token.str.size + 1, "%s", token.str.ptr);
        printf("(%s) ", p);
        free(p);
    } else {
        printf(" ");
    }

}

void pretty_print_tokenlist(TokenList *tokenlist) {
    for (int i = 0; i < tokenlist->size; ++i) {
        pretty_print_token(tokenlist->tokens[i]);
    }
    puts("");
}

void format_token(char *out, Token *tok) {
    if (tok->type < 4) {
        strncpy(out, tok->str.ptr, tok->str.size);
    } else {
        format_token_type(out, tok->type);
    }
}

void format_token_type(char *out, TokenType type) {
    switch (type) {
        case T_OCURLY:
            strcpy(out, "{");
            break;
        case T_CCURLY:
            strcpy(out, "}");
            break;
        case T_OPAREN:
            strcpy(out, "(");
            break;
        case T_CPAREN:
            strcpy(out, ")");
            break;
        case T_SEMICOLON:
            strcpy(out, ";");
            break;
        default:
            strcpy(out, "INVALID TOKEN");
    }
}