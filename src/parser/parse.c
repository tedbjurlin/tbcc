#include <stdlib.h>
#include "parse.h"
#include "ast.h"
#include "../lexer/tokenlist.h"
#include "../util/prettyerror.h"

int expect_token(TokenType type, Token *tok) {
    if (type != tok->type) {
        char typebuf[32];
        char tokbuf[128];
        format_token_type(typebuf, type);
        format_token(tokbuf, tok);
        prettyerror(
            tok->linestart,
            tok->line + 1,
            tok->column,
            "parsing failed: unexpected '%s', expected '%s' instead",
            tokbuf,
            typebuf
        );
    }
    return type == tok->type;
}

AST_Expression *parse_expression(TokenList *tokens) {
    Token *next = next_token(tokens);
    if (!expect_token(T_INTCONST, next)) {
        return NULL;
    }

    AST_Expression *exp = malloc(sizeof(AST_Expression));
    exp->column = next->column;
    exp->constvalue = next->str;
    exp->exptype = AST_IntConst;
    exp->line = next->line;
    exp->linestart = next->linestart;
    return exp;
}

AST_Statement *parse_statement(TokenList *tokens) {
    Token *next = next_token(tokens);
    if (!expect_token(T_RETURN, next)) {
        return NULL;
    }

    AST_Expression *exp = parse_expression(tokens);
    if (!exp) {
        return NULL;
    }

    Token *semi = next_token(tokens);
    if (!expect_token(T_SEMICOLON, semi)) {
        return NULL;
    }

    AST_Statement *stmt = malloc(sizeof(AST_Statement));
    stmt->column = next->column;
    stmt->expression = exp;
    stmt->line = next->line;
    stmt->linestart = next->linestart;
    stmt->stmttype = AST_Return;
    return stmt;
}

AST_Function *parse_function(TokenList *tokens) {
    Token *ret_type = next_token(tokens);
    if (!expect_token(T_INT, ret_type)) {
        return NULL;
    }

    Token *name = next_token(tokens);
    if (name->type != T_IDENTIFIER) {
        char tokbuf[128];
        format_token(tokbuf, name);
        prettyerror(
            name->linestart,
            name->line + 1,
            name->column,
            "parsing failed: unexpected '%s', expected function name instead",
            tokbuf
        );
        return NULL;
    }

    Token *sym = next_token(tokens);
    if (!expect_token(T_OPAREN, sym)) {
        return NULL;
    }
    Token *arg = next_token(tokens);
    if (!expect_token(T_VOID, arg)) {
        return NULL;
    }
    sym = next_token(tokens);
    if (!expect_token(T_CPAREN, sym)) {
        return NULL;
    }
    sym = next_token(tokens);
    if (!expect_token(T_OCURLY, sym)) {
        return NULL;
    }

    AST_Statement *stmt = parse_statement(tokens);
    if (!stmt) {
        return NULL;
    }

    sym = next_token(tokens);
    if (!expect_token(T_CCURLY, sym)) {
        free_statement(stmt);
        return NULL;
    }

    AST_Function *function = malloc(sizeof(AST_Function));
    function->column = ret_type->column;
    function->line = ret_type->line;
    function->linestart = ret_type->linestart;
    function->name = name->str;
    function->statement = stmt;
    return function;
}

/**
 * @brief Expects the end of the token stream.
 * 
 * @param tokens The list of tokens.
 * @returns `0` if eof, `1` otherwise.
 */
int expect_eof(TokenList *tokens) {
    if (tokens->cursor == tokens->size) {
        return 0;
    } else {
        Token *next = peek_token(tokens);
        char tokbuf[128];
        format_token(tokbuf, next);
        prettyerror(
            next->linestart,
            next->line + 1,
            next->column,
            "parsing failed: unexpected '%s', expected EOF instead",
            tokbuf
        );
        return 1;
    }
}

AST_Program *parse(TokenList *tokens) {
    AST_Function *function = parse_function(tokens);
    if (!function) {
        return NULL;
    }
    if (expect_eof(tokens)) {
        free_function(function);
        return NULL;
    };
    AST_Program *program = malloc(sizeof(AST_Program));
    program->function = function;
    return program;
}