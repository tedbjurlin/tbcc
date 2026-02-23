#ifndef AST
#define AST

#include "../util/strwindow.h"

/**
 * The type of an expression.
 */
typedef enum AST_ExpressionType {
    AST_IntConst, // A constant integer expression.
} AST_ExpressionType;

/**
 * @brief Represents an expression.
 * 
 * Contains an expression type, an expression value, and error information.
 * Currently we only support constant integer expressions.
 */
typedef struct AST_Expression {
    StrWindow constvalue; // A window to the constant value.
    char *linestart; // The start of the line in the source.
    int line; // The line number in the source.
    int column; // The column in the source.
    AST_ExpressionType exptype; // The type of the expression.
} AST_Expression;

/**
 * @brief The type of a C statement.
 */
typedef enum AST_StatementType {
    AST_Return // A return statement.
} AST_StatementType;

/**
 * @brief A statement.
 * 
 * Contains an expression, a statement type, and error message information.
 */
typedef struct AST_Statement {
    AST_Expression *expression; // A pointer to an expression.
    char *linestart; // The start of the line in the source.
    int line; // The line number in the source.
    int column; // The column in the source.
    AST_StatementType stmttype; // The type of the statement.
} AST_Statement;

/**
 * @brief A function.
 * 
 * Contains a statement, a function name, and error message info.
 */
typedef struct AST_Function {
    AST_Statement *statement; // A pointer to a statement.
    char *linestart; // The start of the line in the source.
    StrWindow name; // A window to the name in the source.
    int line; // The line number in the source.
    int column; // The column number in the source.
} AST_Function;

/**
 * @brief An abstract syntax tree for a program.
 */
typedef struct AST_Program {
    AST_Function *function; // A pointer to a function.
} AST_Program;

/**
 * @brief Frees an expression.
 */
void free_expression(AST_Expression *expression);

/**
 * @brief Frees a statement.
 */
void free_statement(AST_Statement *statement);

/**
 * @brief Frees a function.
 */
void free_function(AST_Function *function);

/**
 * @brief Frees an AST program.
 */
void free_ast(AST_Program *ast);

/**
 * @brief Pretty prints an AST program for debugging.
 */
void pretty_print_ast(AST_Program *ast);

#endif