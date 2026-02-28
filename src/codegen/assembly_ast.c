#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assembly_ast.h"

// Initial capacity of the instruction list.
const int ILCAP = 16;

/**
* @brief Creates an instruction list.
* 
* Allocates space for `ILCAP` instructions initially, setting size to 0 and
* capacity ot `ILCAP`.
* 
* @returns A pointer to the newly created list.
*/
ASM_AST_InstructionList *create_instructionlist() {
  ASM_AST_InstructionList *list = malloc(sizeof(ASM_AST_InstructionList));
  list->instructions = malloc(sizeof(ASM_AST_Instruction) * ILCAP);
  list->capacity = ILCAP;
  list->size = 0;
  return list;
}

/**
* @brief Frees a list of instructions.
* 
* Frees each operation in each instruction, then frees the list of instruction,
* then finally the data structure itself.
* 
* @param list A pointer to the list to be freed.
*/
void free_instructionlist(ASM_AST_InstructionList *list) {
  for (int i = 0; i < list->size; ++i) {
    free((list->instructions)[i].op1);
    free((list->instructions)[i].op2);
    free((list->instructions)[i].op3);
  }
  free(list->instructions);
  free(list);
}

/**
* @brief Frees an ASM_AST program.
* 
* Frees the instruction list of the main function, then frees the function and
* the program.
* 
* @param prog A pointer to the program to be freed.
*/
void free_asm_ast(ASM_AST_Program *prog) {
  free_instructionlist(prog->func->instructions);
  free(prog->func);
  free(prog);
}

/**
* @brief Inserts an instruction into the instruction list.
* 
* If the list is full, reallocates space, doubling the capacity of the list.
* 
* @param list A pointer to the instruction list.
* @param instruction The instruction to be inserted.
*/
void insert_instruction(ASM_AST_InstructionList *list, ASM_AST_Instruction instruction) {
  if (list->size == list->capacity) {
    list->capacity *= 2;
    list->instructions = realloc(list->instructions, list->capacity);
  }
  list->instructions[list->size] = instruction;
  ++list->size;
}

void pretty_print_operand(ASM_AST_Operand *op) {
  if (op) {
    switch (op->type) {
      case ASM_AST_IMM:
      char val[128] = {'\0'};
      strncpy(val, op->immvalue.ptr, op->immvalue.size);
      printf("IMM(%s)", val);
      break;
      case ASM_AST_REGISTER:
      printf("REGISTER");
    }
  }
  
}

void pretty_print_instructions(ASM_AST_InstructionList *list) {
  if (list) {
    for (int i = 0; i < list->size; ++i) {
      switch (list->instructions[i].type) {
        case ASM_AST_RET:
        printf("  RET\n");
        break;
        case ASM_AST_MOV:
        printf("  MOV(");
        pretty_print_operand(list->instructions[i].op1);
        printf(", ");
        pretty_print_operand(list->instructions[i].op2);
        printf(")\n");
      }
    }
  }
}

void pretty_print_asm_ast(ASM_AST_Program *asm_ast) {
  if (asm_ast && asm_ast->func) {
    char name[128] = {'\0'};
    strncpy(name, asm_ast->func->name.ptr, asm_ast->func->name.size);
    printf("Function %s:\n", name);
    pretty_print_instructions(asm_ast->func->instructions);
  }
}