#include "assembly.h"
#include <stdio.h>
#include <string.h>
#include "../codegen/codegen.h"

char printbuf[128];

/**
* @brief Generates the assmebly of an operand.
* 
* Prints the operand without a newline.
* @param outifle The file handle of the output file.
* @param op The operand.
* @returns `0` on success, `1` otherwise.
*/
int emit_operand(FILE *outfile, ASM_AST_Operand *op) {
  if (!op) {
    return 1;
  }
  
  switch (op->type) {
    case ASM_AST_REGISTER:
    fprintf(outfile, "%%eax");
    break;
    case ASM_AST_IMM:
    strncpy(printbuf, op->immvalue.ptr, op->immvalue.size);
    printbuf[op->immvalue.size] = '\0';
    fprintf(outfile, "$%s", printbuf);
    break;
    default:
    return 1;
  }
  return 0;
}

/**
* @brief Generates the assembly of a single instruciton.
* 
* @param outfile The file handle of the output file.
* @param instruction The instruction.
* @returns `0` on success, `1` otherwise.
*/
int emit_instruction(FILE *outfile, ASM_AST_Instruction *instruction) {
  if (!instruction) {
    return 1;
  }
  switch (instruction->type) {
    case ASM_AST_MOV:
    fprintf(outfile, "    movl    ");
    emit_operand(outfile, instruction->op1);
    fprintf(outfile, ", ");
    emit_operand(outfile, instruction->op2);
    putc('\n', outfile);
    break;
    case ASM_AST_RET:
    fprintf(outfile, "    ret\n");
    break;
    default:
    return 1;
  }
  return 0;
}

/**
* @brief Generates assembly of a function into the outfile.
* 
* @param outfile The file handle of the output file.
* @param func A pointer to the function.
* @returns `0` if success, `1` otherwise.
*/
int emit_function(FILE *outfile, ASM_AST_Function *func) {
  if (!func) {
    return 1;
  }
  
  strncpy(printbuf, func->name.ptr, func->name.size);
  printbuf[func->name.size] = '\0';
  fprintf(outfile, "    .globl  %s\n", printbuf);
  fprintf(outfile, "    .type   %s, @function\n", printbuf);
  fprintf(outfile, "%s:\n", printbuf);
  
  for (int i = 0; i < func->instructions->size; i++) {
    if (emit_instruction(outfile, func->instructions->instructions + i)) {
      return 1;
    }
  }
  
  return 0;
}

int emit_assembly(FILE *outfile, ASM_AST_Program *prog) {
  if (!outfile || !prog) {
    return 1;
  }
  
  if (emit_function(outfile, prog->func)) {
    return 1;
  }
  
  fprintf(outfile, "	  .section    .note.GNU-stack,\"\",@progbits\n");
  return 0;
}