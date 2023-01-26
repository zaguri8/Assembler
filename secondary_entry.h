/*
//  secondary_entry.h
//  Assembler
//
//  Created by Nadav Avnon on 08/01/2023.
*/

#ifndef secondary_entry_h
#define secondary_entry_h

#include <stdio.h>
#include "structs.h"
void encode_instruction(sentence * curr);
void encode_immediate_operand(int oper);
int encode_jump(sentence *currsentence);
void encode_direct_operand(char * oper);
void encode_extern_operand(void);
void create_object_file(void);
memory_word * create_new_memory_word(void);
void encode_register(char * register_names[], char * operand_type);
int open_secondary_entry_output_files(void);
void close_secondary_entry_output_files(void);
int handle_direct_operands(char * oper);
void add_item_to_code_list(memory_word * item);
void append_data_table_into_end_of_code_table(memory_word * data_head);
void ExecuteSecondEntry(char * filename);
#endif /* secondary_entry_h */
