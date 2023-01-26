#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "structs.h"
#include "pre_assembler.h"
#define MAX_LINE_LENGTH 1000



int PreAssembler_DeleteFile(const char *filename)
{
    remove(filename);
    return 0;
}


int PreAssembler_CloseFiles_Success(FILE *file)
{
    if (fclose(file) == EOF)
    {
        printf("[Error] error closing file.\n");
    }
    return 1;
}
int PreAssembler_CloseFile_Failure(FILE *file, const char* file_name)
{
    PreAssembler_CloseFiles_Success(file);
    return PreAssembler_DeleteFile(file_name);
}


int PreAssembler_CompareStrings_IgnoreSpace(const char* s1, const char* s2) {
    int i = 0, j = 0;

    /* Skip leading spaces in both strings */
    while (isspace(s1[i])) {
        i++;
    }
    while (isspace(s2[j])) {
        j++;
    }

    /* Compare the rest of the strings */
    while (s1[i] != '\0' && s2[j] != '\0') {
        if (s1[i] != s2[j]) {
            return s1[i] - s2[j];
        }
        i++;
        j++;
    }

    /* Check for trailing spaces in both strings */
    while (isspace(s1[i])) {
        i++;
    }
    while (isspace(s2[j])) {
        j++;
    }

    /* If both strings are equal, return 0 */
    if (s1[i] == '\0' && s2[j] == '\0') {
        return 0;
    }
    /* If s1 is longer than s2, return 1 */
    if (s1[i] != '\0') {
        return 1;
    }
    /* If s2 is longer than s1, return -1 */
    return -1;
}

int isLanguageSavedWord(char *word)
{
    int i;
    const int SAVED_WORDS_LEN = 28;
    for (i = 0; i < SAVED_WORDS_LEN; i++)
    {
        if (PreAssembler_CompareStrings_IgnoreSpace(word, saved_languages_words[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}


void PreAssembler_ReplaceExtension(char *filename, char *new_ext, char *new_filename)
{
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return;
    strcpy(new_filename, filename);
    new_filename[dot - filename + 1] = '\0';
    strcat(new_filename, new_ext);
}

int PreAssembler_ReplaceMacros(const char* input_file,const char* output_file) {


    struct Macro macros[MAX_MACROS];
    char line[1000];
    int i;
    int line_num = 1;
    int num_macros = 0;
    int in_macro = 0; /* flag to check if we are currently in a macro definition */
    FILE *input, *output;
    input = fopen(input_file, "r");
    if (input == NULL) {
        printf("[Error] error opening input file.\n");
        return 0;
    }
    /* open output file */
    output = fopen(output_file, "w");
    if (output == NULL) {
        printf("[Error] error opening output file.\n");
        return PreAssembler_CloseFile_Failure(input, output_file);
    }

    while (fgets(line, sizeof(line), input) != NULL) {
        /* Check if the line contains a macro definition */
        if (strncmp(line, "mcr", 3) == 0) {
            char *name = strtok(line + 3, " ");
            /* set in_macro flag to true */
            in_macro = 1;
            /* initialize value with empty string */
            strcpy(macros[num_macros].value, "");
            strcpy(macros[num_macros].name, name);
            /* make sure the name is not a keyword */
            if(isLanguageSavedWord(macros[num_macros].name)){
                printf("[Error] macro name is a keyword. at line %d\n", line_num);
                fclose(output);
                return PreAssembler_CloseFile_Failure(input, output_file);
            } else if(strlen(macros[num_macros].name) == 0){
                printf("[Error] macro name is empty. at line %d\n", line_num);
                fclose(output);
                return PreAssembler_CloseFile_Failure(input, output_file);
            }
            /* check if it is already defined */
            for (i = 0; i < num_macros; i++) {
                if (PreAssembler_CompareStrings_IgnoreSpace(macros[num_macros].name, macros[i].name) == 0) {
                    printf("[Error] macro name %s is already defined. problematic line: %d\n", macros[num_macros].name, line_num);
                    fclose(output);
                    return PreAssembler_CloseFile_Failure(input, output_file);
                }
            }
        } else if (strncmp(line, "endmcr", 6) == 0) {
            /* set in_macro flag to false */
            in_macro = 0;
            num_macros++;
        } else if (in_macro) {
            /* if we are currently in a macro definition, append the line to the value */
            printf("Appending %s to macro %s at line %d \n", line, macros[num_macros].name, line_num);
            strcat(macros[num_macros].value, line);
        } else {
            /* Iterate through each word of the line */
            char *word = strtok(line, " ");
            while (word != NULL) {
                /* Check if the word matches a macro name */
                for (i = 0; i < num_macros; i++) {
                    if (PreAssembler_CompareStrings_IgnoreSpace(word, macros[i].name) == 0) {
                        word = malloc(strlen(macros[i].value) + 1);
                        /* Replace the word with the corresponding macro value */
                        printf("Replacing %s with %s at line %d \n", word, macros[i].value, line_num);
                        strcpy(word, macros[i].value);
                        break;
                    }
                }
                /* write the word to the output file */
                if(!isspace(word[strlen(word) - 1]))
                    fprintf(output, "%s ", word);
                else
                    fprintf(output, "%s" , word);    
                word = strtok(NULL, " ");
            }
        }
        line_num++;
    }
    fclose(output);
    return PreAssembler_CloseFiles_Success(input);
}

int PreAssembler_Execute(char *src_file)
{
    char *new_ext = ASSEMBLY_FILE_EXTENSION;
    char *new_filename = malloc(strlen(src_file) + strlen(new_ext) + 1);
    PreAssembler_ReplaceExtension(src_file, new_ext, new_filename);
    return PreAssembler_ReplaceMacros(src_file, new_filename);
}

