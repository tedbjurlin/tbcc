#include <stdlib.h>
#include "aerate.h"
#include "air.h"
#include "../parser/ast.h"
#include "../util/prettyerror.h"

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

int aerate_return(AIR_InstructionList *list, AST_Expression *expression) {
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

AIR_InstructionList *aerate_instructions(AST_Statement *stmt) {
    AIR_InstructionList *list = create_instructionlist();
    switch (stmt->stmttype) {
        case AST_Return:
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

AIR_Function *aerate_function(AST_Function *func) {
    AIR_InstructionList *list = aerate_instructions(func->statement);

    AIR_Function *rfunc = malloc(sizeof(AIR_Function));
    rfunc->name = func->name;
    rfunc->instructions = list;
    return rfunc;
}

AIR_Program *aerate(AST_Program *prog) {
    AIR_Function *func = aerate_function(prog->function);
    if (!func) {
        return NULL;
    }

    AIR_Program *rprog = malloc(sizeof(AIR_Program));
    rprog->func = func;
    return rprog;
}