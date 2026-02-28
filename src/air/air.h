#ifndef AIR
#define AIR

#include "../util/strwindow.h"

/**
* @brief The type of an operand to an assembly instruction.
*/
typedef enum AIR_OperandType {
  AIR_REGISTER, // A register.
  AIR_IMM, // An immediate value.
} AIR_OperandType;

/**
* @brief The operand to an assembly instruction.
*/
typedef struct AIR_Operand {
  StrWindow immvalue; // (Optional) A window to the immediate value in the source file.
  AIR_OperandType type; // The type of the operand.
} AIR_Operand;

/**
* @brief The type of an assembly instruction.
*/
typedef enum AIR_InstructionType {
  AIR_MOV, // A move instruction.
  AIR_RET, // A return instruction.
} AIR_InstructionType;

/**
* @brief An assembly instruction.
*/
typedef struct AIR_Instruction {
  AIR_Operand *op1; // (Optional) The first operand of the instruction.
  AIR_Operand *op2; // (Optional) The second operand of the instruction.
  AIR_Operand *op3; // (Optional) The third operand of the instruction.
  AIR_InstructionType type; // The type of the instruction.
} AIR_Instruction;

/**
* @brief A dynamically-sized list of assembly instructons.
*/
typedef struct AIR_InstructionList {
  AIR_Instruction *instructions; // Array of instructions.
  int size; // Number of isntructions in the array.
  int capacity; // Total size of array currently allocated.
} AIR_InstructionList;

/**
* @brief An assembly function.
*/
typedef struct AIR_Function {
  AIR_InstructionList *instructions; // A list of the instructions in the function.
  StrWindow name; // A window to the name of the function in the source file.
} AIR_Function;

/**
* @brief The root node of the Assembly Intermediate Representation.
*/
typedef struct AIR_Program {
  AIR_Function *func;
} AIR_Program;

/**
* @brief Creates a dynamically-sized list of instructions.
* @returns A pointer to the instruction list.
*/
AIR_InstructionList *create_instructionlist();

/**
* Frees the memory of an instruction list.
* 
* Does not free `StrWindow` members of instructions since they use the memory
* of the source file.
* @param list A pointer to the instruction list to be freed.
*/
void free_instructionlist(AIR_InstructionList *list);

/**
* @brief Frees the memory of an AIR.
* 
* Does not free `StrWindow` members of the AIR since they use the memory of the
* source file.
* @param prog A pointer to the AIR to be freed.
*/
void free_air(AIR_Program *prog);

/**
* @brief Inserts an instruction into an list of instructions.
* 
* The instruction is inserted at the end of the target list. 
* @param list A pointer to the list of instructions.
* @param instruction The instruction to be inserted.
*/
void insert_instruction(AIR_InstructionList *list, AIR_Instruction instruction);

/**
* @brief Pretty prints an AIR.
* 
* Designed for debugging purposes. The AIR is printed to stdout.
* @param prog A pointer to the AIR to be printed.
*/
void pretty_print_air(AIR_Program *prog);

#endif