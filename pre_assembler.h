/*
//  pre_assembler.h
//  Assembler
//
//  Created by Nadav Avnon on 15/01/2023.
*/

#ifndef pre_assembler_h
#define pre_assembler_h
#include <stdio.h>
#include "structs.h"
#define ASSEMBLY_FILE_EXTENSION "am"
#define MACRO_READ_MODE 1
#define READ_MODE 0
#define MAX_MACROS 100
#define MODE_REMOVE 1
#define MODE_IDLE 0

struct Macro {
    char name[100];
    char value[1000];
};
/* a function to detect macros and replace them with their definition */
int PreAssembler_ReplaceMacros(const char* input_file,const char* output_file);
void PreAssembler_ReplaceExtension(char* filename, char* new_ext, char* new_filename);
int PreAssembler_CloseFiles_Success(FILE *file);
int PreAssembler_CompareStrings_IgnoreSpace(const char* s1, const char* s2);
int isLanguageSavedWord(char* word);
int PreAssembler_Execute(char* src_file);
#endif /* pre_assembler_h */
