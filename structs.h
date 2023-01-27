/*
//  structs.h
//  Assembler
//
//  Created by Nadav Avnon on 01/01/2023.
*/


#ifndef structs_h
#define structs_h
#include <string.h>

#define MAX_DATA_ARR_SIZE 10
#define NUM_OF_OPERAND_TYPES 4
#define NUM_OF_SAVED_WORDS 29
#define NUM_OF_OPCODES 16
#define MAX_STRING_SIZE 80
#define MAX_SYMBOL_SIZE 31
#define MAX_OPCODE_SIZE 5
#define MAX_OPERAND_TYPE_SIZE 3
#define IMMEDIATE_OPERAND_TYPE "00"
#define DIRECT_OPERAND_TYPE "01"
#define JUMP_OPERAND_TYPE "10"
#define REGISTER_OPERAND_TYPE "11"

enum data_type {NONE, DATA, CODE};
enum boolean {FALSE, TRUE};
enum guidance {EXTERN, ENTRY, NUM, STRING};



/* struct for holding an opcode and it's binary value */
typedef struct opcodes {
    char* opcode;
    char binary_val[5];
    int qty_of_supported_operands;
    int source_operand_types[5];
    int destination_operand_types[5];
} opcodes;

typedef struct mem_words_per_operand_type {
    char operand_type[2];
    int num_of_mem_words;
} mem_words_per_operand_type;

/* struct for holding a register and it's binary value */
typedef struct registers {
    char register_name[3];
    char register_val[5];
} registers;

typedef struct symbol_line {
    char symbol[MAX_SYMBOL_SIZE];
    int address;
    int is_extern;
    int symbol_type; /* from enum: NONE, DATA, CODE */
    struct symbol_line *next;
} symbol_line;

/* struct for holding the IC \ DC (code memory word address \ data memory word address), an array of 13 bits (in chars), and a pointer to the next struct of the same type */
typedef struct memory_word {
    int address;
    char bits[14]; /* there are 14 places so '\0' can be added */
    struct memory_word *next;
} memory_word;


typedef struct sentence {
    int is_action; /* 1 if it's an action sentence, 0 if it's a guidance sentence */
    int is_store_command; /* 1 if it's .data/.string., 0 if it's extern or entry */
    int guidance_command; /* enum: EXTERN / ENTRY / NUM (=DATA)  / STRING  */
    int is_symbol;
    int is_jump;
    char symbol[MAX_SYMBOL_SIZE];
    char opcode[MAX_OPCODE_SIZE];
    int num_of_operands; /* 0 if no operands, 1 if destiantion, 2 if if both destination and source*/
    char source_operand_type[MAX_OPERAND_TYPE_SIZE]; /* 3 places so '\0' can be added - miunim */
    char dest_operand_type[MAX_OPERAND_TYPE_SIZE];   /* 3 places so '\0' can be added - miunim */
    char operand_1[MAX_SYMBOL_SIZE]; /* for variables, registers */
    char operand_2[MAX_SYMBOL_SIZE]; /* for variables, registers */
    char jump_param_1[MAX_SYMBOL_SIZE];
    char jump_param_2[MAX_SYMBOL_SIZE];
    char jump_param_1_type[3]; /* 0, 1, 2 */ /*  symbol, immediate, register  */
    char jump_param_2_type[3]; /* 0, 1, 2*/ /*  symbol, immediate, register  */
    int immediate_operand_a; /* when we have "#" */
    int immediate_operand_b; /* when we have "#" */
    char string[MAX_STRING_SIZE]; /* if guidance_command  = .string, check for it's value in this field */
    int data_arr[MAX_DATA_ARR_SIZE]; /* if guidance command = .data, check for the values in this array */
    int data_arr_num_of_params; /* how many numbers to take from data_arr */
    struct sentence *next;
} sentence;


extern opcodes opcodes_table[];
extern mem_words_per_operand_type operands_vs_num_of_words_to_use[];
extern registers registers_table[];
extern char *saved_languages_words[NUM_OF_SAVED_WORDS];
int registers_table_length;
 
#endif


