#include <stdlib.h>
#include "aerate.h"
#include "air.h"
#include "../parser/ast.h"
#include "../util/prettyerror.h"

/**
* @brief Construct the AIR of an expression.
* 
* Constructs an `AIR_Instruction` from the current expression and inserts it
* into the instruction list.
* 
* @param list A pointer to the list of instructions.
* @param expression A pointer to the current expression.
* @returns `0` if it the expression is valid, '1' otherwise.
*/
int aerate_expression(AIR_InstructionList *list, AST_Expression *expression) {
  switch (expression->exptype) {
    case AST_IntConst:
    AIR_Operand *op1 = malloc(sizeof(AIR_Operand));
    AIR_Operand *op2 = malloc(sizeof(AIR_Operand));
    op1->type = AIR_IMM;
    op1->immvalue = expression->constvalue;
    op2->type = AIR_REGISTER;
    insert_instruction(
      list,
      (AIR_Instruction){
        .type = AIR_MOV,
        .op1 = op1,
        .op2 = op2,
      }
    );
    break;
    default:
    prettyerror(
      expression->linestart,
      expression->line,
      expression->column,
      "parsing produced invalid expression"
    );
    return 1;
  }
  return 0;
}

/**
* @brief Inserts a return AIR into the instruction list following its expression.
* 
* @param list A pointer to the list of instructions.
* @param expression A pointer to the current expression.
* @returns `0` if the expression is valid, `1` otherwise.
*/
int aerate_return(AIR_InstructionList *list, AST_Expression *expression) {
  // return 1 if aerate_expression returns 1
  if (aerate_expression(list, expression)) {
    return 1;
  }
  insert_instruction(
    list,
    (AIR_Instruction){
      .type = AIR_RET,
    }
  );
  return 0;
}

/**
* @brief Constructs a list of the instructions that correspond to a statement.
* 
* @param stmt A pointer to the current statement.
* @returns A pointer to the list of generated instructions. `NULL` on failure.
*/
AIR_InstructionList *aerate_instructions(AST_Statement *stmt) {
  AIR_InstructionList *list = create_instructionlist();
  switch (stmt->stmttype) {
    case AST_Return:
    // aerate_return returns 1 if it fails
    if (aerate_return(list, stmt->expression)) {
      free_instructionlist(list);
      return NULL;
    };
    break;
    default:
    prettyerror(
      stmt->linestart,
      stmt->line,
      stmt->column,
      "parsing produced invalid statement"
    );
    free_instructionlist(list);
    return NULL;
  }
  return list;
}

/**
* @brief Constructs the AIR for a function.
* 
* @param func A pointer to the current function.
* @returns A pointer to the AIR function.
*/
AIR_Function *aerate_function(AST_Function *func) {
  // no error on null instructions because functions are allowed to be empty.
  // this may be wrong from the assembler perspective. I will readdress it
  // when I start working on typechecking.
  AIR_InstructionList *list = aerate_instructions(func->statement);
  
  AIR_Function *rfunc = malloc(sizeof(AIR_Function));
  rfunc->name = func->name;
  rfunc->instructions = list;
  return rfunc;
}

/**
* @brief Convert a program into the assembler intermediate representation.
* 
* @param prog A pointer to the program.
* @returns A pointer to the AIR for the program. NULL on failure.
*/
AIR_Program *aerate(AST_Program *prog) {
  AIR_Function *func = aerate_function(prog->function);
  if (!func) {
    return NULL;
  }
  
  AIR_Program *rprog = malloc(sizeof(AIR_Program));
  rprog->func = func;
  return rprog;
}