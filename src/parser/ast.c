#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"

void free_expression(AST_Expression *expression) {
    free(expression);
}

void free_statement(AST_Statement *statement) {
    if (statement->expression) {
        free_expression(statement->expression);
    }
    free(statement);
}

void free_function(AST_Function *function) {
    if (function->statement) {
        free_statement(function->statement);
    }
    free(function);
}

void free_ast(AST_Program *ast) {
    if (ast->function) {
        free_function(ast->function);
    }
    free(ast);
}

void pretty_print_expression(AST_Expression *exp, int level) {
    switch (exp->exptype) {
        case AST_IntConst:
            char con[128] = {'\0'};
            strncpy(con, exp->constvalue.ptr, exp->constvalue.size);
            printf("%*sIntConst %s\n", level, "", con);
            break;
        default:
            printf("%*sINVALID EXPRESSION\n", level, "");
    }
}

char *pretty_statement_type(AST_StatementType stmttype) {
    switch (stmttype) {
        case AST_Return:
            return "Return";
        default:
            return "INVALID STATEMENT TYPE";
    }
}

void pretty_print_statement(AST_Statement *statement, int level) {
    printf("%*sStatement %s(\n",  level, "", pretty_statement_type(statement->stmttype));
    pretty_print_expression(statement->expression, level + 2);
    printf("%*s)\n", level, "");
}

void pretty_print_function(AST_Function *function, int level) {
    char name[128] = {'\0'};
    strncpy(name, function->name.ptr, function->name.size);
    printf("%*sFunction %s(\n", level, "", name);
    pretty_print_statement(function->statement, level + 2);
    printf("%*s)\n", level, "");
}

void pretty_print_ast(AST_Program *ast) {
    pretty_print_function(ast->function, 0);
}