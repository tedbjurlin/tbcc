#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "air.h"

const int ILCAP = 16;

AIR_InstructionList *create_instructionlist() {
    AIR_InstructionList *list = malloc(sizeof(AIR_InstructionList));
    list->instructions = malloc(sizeof(AIR_Instruction) * ILCAP);
    list->capacity = ILCAP;
    list->size = 0;
    return list;
}

void free_instructionlist(AIR_InstructionList *list) {
    for (int i = 0; i < list->size; ++i) {
        free((list->instructions)[i].op1);
        free((list->instructions)[i].op2);
        free((list->instructions)[i].op3);
    }
    free(list->instructions);
    free(list);
}

void free_air(AIR_Program *prog) {
    free_instructionlist(prog->func->instructions);
    free(prog->func);
    free(prog);
}

void insert_instruction(AIR_InstructionList *list, AIR_Instruction instruction) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->instructions = realloc(list->instructions, list->capacity);
    }
    list->instructions[list->size] = instruction;
    ++list->size;
}

void pretty_print_operand(AIR_Operand *op) {
    if (op) {
        switch (op->type) {
            case AIR_IMM:
                char val[128] = {'\0'};
                strncpy(val, op->immvalue.ptr, op->immvalue.size);
                printf("IMM(%s)", val);
                break;
            case AIR_REGISTER:
                printf("REGISTER");
        }
    }

}

void pretty_print_instructions(AIR_InstructionList *list) {
    if (list) {
        for (int i = 0; i < list->size; ++i) {
            switch (list->instructions[i].type) {
                case AIR_RET:
                    printf("  RET\n");
                    break;
                case AIR_MOV:
                    printf("  MOV(");
                    pretty_print_operand(list->instructions[i].op1);
                    printf(", ");
                    pretty_print_operand(list->instructions[i].op2);
                    printf(")\n");
            }
        }
    }
}

void pretty_print_air(AIR_Program *air) {
    if (air && air->func) {
        char name[128] = {'\0'};
        strncpy(name, air->func->name.ptr, air->func->name.size);
        printf("Function %s:\n", name);
        pretty_print_instructions(air->func->instructions);
    }
}