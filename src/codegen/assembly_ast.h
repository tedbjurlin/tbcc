#ifndef ASSEMBLYAST
#define ASSEMBLYAST

#include "../util/strwindow.h"

/**
* @brief The type of an operand to an assembly instruction.
*/
typedef enum ASM_AST_OperandType {
  ASM_AST_REGISTER, // A register.
  ASM_AST_IMM, // An immediate value.
} ASM_AST_OperandType;

/**
* @brief The operand to an assembly instruction.
*/
typedef struct ASM_AST_Operand {
  StrWindow immvalue; // (Optional) A window to the immediate value in the source file.
  ASM_AST_OperandType type; // The type of the operand.
} ASM_AST_Operand;

/**
* @brief The type of an assembly instruction.
*/
typedef enum ASM_AST_InstructionType {
  ASM_AST_MOV, // A move instruction.
  ASM_AST_RET, // A return instruction.
} ASM_AST_InstructionType;

/**
* @brief An assembly instruction.
*/
typedef struct ASM_AST_Instruction {
  ASM_AST_Operand *op1; // (Optional) The first operand of the instruction.
  ASM_AST_Operand *op2; // (Optional) The second operand of the instruction.
  ASM_AST_Operand *op3; // (Optional) The third operand of the instruction.
  ASM_AST_InstructionType type; // The type of the instruction.
} ASM_AST_Instruction;

/**
* @brief A dynamically-sized list of assembly instructons.
*/
typedef struct ASM_AST_InstructionList {
  ASM_AST_Instruction *instructions; // Array of instructions.
  int size; // Number of isntructions in the array.
  int capacity; // Total size of array currently allocated.
} ASM_AST_InstructionList;

/**
* @brief An assembly function.
*/
typedef struct ASM_AST_Function {
  ASM_AST_InstructionList *instructions; // A list of the instructions in the function.
  StrWindow name; // A window to the name of the function in the source file.
} ASM_AST_Function;

/**
* @brief The root node of the Assembly Intermediate Representation.
*/
typedef struct ASM_AST_Program {
  ASM_AST_Function *func;
} ASM_AST_Program;

/**
* @brief Creates a dynamically-sized list of instructions.
* @returns A pointer to the instruction list.
*/
ASM_AST_InstructionList *create_instructionlist();

/**
* Frees the memory of an instruction list.
* 
* Does not free `StrWindow` members of instructions since they use the memory
* of the source file.
* @param list A pointer to the instruction list to be freed.
*/
void free_instructionlist(ASM_AST_InstructionList *list);

/**
* @brief Frees the memory of an ASM_AST.
* 
* Does not free `StrWindow` members of the ASM_AST since they use the memory of the
* source file.
* @param prog A pointer to the ASM_AST to be freed.
*/
void free_asm_ast(ASM_AST_Program *prog);

/**
* @brief Inserts an instruction into an list of instructions.
* 
* The instruction is inserted at the end of the target list. 
* @param list A pointer to the list of instructions.
* @param instruction The instruction to be inserted.
*/
void insert_instruction(ASM_AST_InstructionList *list, ASM_AST_Instruction instruction);

/**
* @brief Pretty prints an ASM_AST.
* 
* Designed for debugging purposes. The ASM_AST is printed to stdout.
* @param prog A pointer to the ASM_AST to be printed.
*/
void pretty_print_asm_ast(ASM_AST_Program *prog);

#endif