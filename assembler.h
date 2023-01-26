/*
//  assembler.h
//  Assembler
//
//  Created by Nadav Avnon on 02/01/2023.
*/

#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include <stdio.h>
#include "structs.h"
#define CODE_TABLE_START_ADDRESS 100
#define OPCODES_TABLE_LENGTH 15

int IC;
int DC;
symbol_line *symbol_tail;
symbol_line *symbol_head;
memory_word *data_tail;
memory_word *data_head;
memory_word *code_tail;
memory_word *code_head;
sentence *sentence_head;
sentence *sentence_tail;

void free_data(memory_word *data_head);
void free_symbol(symbol_line *symbol_head);
void free_sentence(sentence *sentence_head);
void free_code(memory_word *code_head);
void free_secondary_entry_data_structs_linked_lists(void);
int ExecuteInitialEntry(FILE *fd);
int execute_pre_assembler(FILE *fp, char *file_name);
void ExecuteSecondEntry(char *filename);
void _print(char*label, char* any);
void _print_num(char*label, int any);
#endif
