/*
//  structs.c
//  Assembler
//
//  Created by Nadav Avnon on 01/01/2023.
*/

#include "structs.h"

opcodes opcodes_table[] = {
    { "mov", "0000",2,{0,1,2,3,-1},{1,2,3,-1}},
    { "cmp", "0001",2,{0,1,2,3,-1},{0,1,2,3,-1}},
    { "add", "0010",2,{0,1,2,3,-1},{1,2,3,-1}},
    { "sub", "0011",2,{0,1,2,3,-1},{1,2,3,-1}},
    { "not", "0100",1,{-1},{1,2,3,-1}},
    { "clr", "0101",1,{-1},{1,2,3,-1}},
    { "lea", "0110",2,{1,2,-1},{1,2,3,-1}},
    { "inc", "0111",1,{-1},{1,2,3,-1}},
    { "dec", "1000",1,{-1},{1,2,3,-1}},
    { "jmp", "1001",1,{-1},{1,2,3,-1}},
    { "bne", "1010",1,{-1},{1,2,3,-1}},
    { "red", "1011",1,{-1},{1,2,3,-1}},
    { "prn", "1100",1,{-1},{0,1,2,3,-1}},
    { "jsr", "1101",1,{-1},{1,2,3,-1}},
    { "rts", "1110",0,{-1},{-1}},
    { "stop", "1111",0,{-1},{-1}}
};
/*
 char* opcode;
 char binary_val[5];
 int qty_of_supported_operands;
 int source_operand_types[5];
 int destination_operand_types[5];
 */

/* in the table: how many memory words the operand type requires */
mem_words_per_operand_type operands_vs_num_of_words_to_use[] = {
    {IMMEDIATE_OPERAND_TYPE, 1},
    {DIRECT_OPERAND_TYPE, 1},
    {REGISTER_OPERAND_TYPE, 1}
};

registers registers_table[] = {
    {"r0", "0000"},
    {"r1", "0001"},
    {"r2", "0010"},
    {"r3", "0011"},
    {"r4", "0100"},
    {"r5", "0101"},
    {"r6", "0110"},
    {"r7", "0111"}
};

int registers_table_length = sizeof(registers_table)/sizeof(registers_table[0]);

char *saved_languages_words[NUM_OF_SAVED_WORDS] = {".extern", ".entry", ".data", ".string", "mov", "cmp", "add", "sub", "not", "clr", "lea",
                    "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

