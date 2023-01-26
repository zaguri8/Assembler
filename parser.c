/*
//  parser.c
//  Assembler
//
//  Created by Nadav Avnon on 01/01/2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"
#include "assembler.h"
#include "converters.h"

#define MAX_DIGITS_SIZE 5

/* strcmp_lower -
receives: 2 strings for non-case-sensitive strcmp.
returns: strcmp result on tolower(word1), word2.
*/
int strcmp_lower(char *word1, char *word2)
{
    int i = 0;
    char word1_lower[MAX_STRING_SIZE];
    while (word1[i] != '\0')
    {
        word1_lower[i] = tolower(word1[i]);
        i++;
    }
    word1_lower[i] = '\0';
    if (!word2)
        return 1;
    return strcmp(word1_lower, word2);
}

/* is_current_word_empty -
 receives: the current word obtained from list.
 returns: 1 if the word is empty, 0 if not.
 NOTE: The used of this function is in order to handle empty spaces or tabs between words in the line from the source file. */
int is_current_word_empty(char *word)
{
    return strlen(word) == 0 ? TRUE : FALSE;
}

/* get_next_word -
 receives: a pointer to the line to take the word from, a pointer to the current_word, and the position in the line where last time the function was called returned.
 returns: the position where we stopped in the line.
 NOTE: in case of ":" or "." - these will be returned as part of the word. The reason is that these characters will assist
 with symbol type and commands recognition. They must be removed afterwards, in other function.
 NOTE2: The parameter last_position should be -1 when a new line is parsed. */
int get_next_word(char *current_word, char *line, int last_position)
{
    int position = last_position + 1;
    int i = 0;

    if (line[position] == '\n' || line[position] == EOF || line[position] == '\0')
    {
        current_word[i] = '\0';
        return position;
    }

    while (line[position] != ' ' && line[position] != ',' && line[position] != '(' && line[position] != ')' && line[position] != '\n' && line[position] != EOF && line[position] != '\0' && line[position] != '\t')
    {
        current_word[i] = line[position];
        i++;
        position++;
    }
    current_word[i] = '\0';

    if (is_current_word_empty(current_word))
        position = get_next_word(current_word, line, position);

    return position;
}

/* is_saved_word -
   receives: the current word to check if it is a saved word in the language or not.
   returns 1 if the word is a saved word, 0 if not. */
int is_saved_word(char *current_word, int line_number, int *syntax_errors, int print_err)
{
    int i;

    for (i = 0; i < NUM_OF_SAVED_WORDS; i++)
    {
        if (strcmp_lower(current_word, saved_languages_words[i]) == 0)
        {
            if (print_err) /*print error will also set syntax errors flag if a the symbol is forbidden to be a saved word*/
            {
                printf("Error in line %d - the word %s is a saved word in the language\n", line_number, current_word);
                *syntax_errors = TRUE;
            }

            return TRUE;
        }
    }

    return FALSE;
}

/* is alphanumeric
receivces: string
returns: TRUE if string consists only alphanumeric characters, else false*/

int is_alphanumeric(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (!isalnum(str[i]))
            return FALSE;
        i++;
    }

    return TRUE;
}
/* is_command -
 */
int is_command(char *curr_word)
{
    if (strcmp_lower(curr_word, ".data") == 0 || strcmp_lower(curr_word, ".string") == 0 || strcmp_lower(curr_word, ".entry") == 0 || strcmp_lower(curr_word, ".extern") == 0)
        return TRUE;
    else
        return FALSE;
}

/* is_symbol -
 receives: the current word to parse,
 returns: 1 if it's a symbol, 0 if not. */
int is_symbol(char *current_word, int line_number, int *syntax_errors, int begining_of_sentence, int print_err)
{
    size_t length = strlen(current_word);

    if (!isalpha(current_word[0]) && !is_command(current_word))
    {
        if (current_word[0] == '.')
        {
            fprintf(stderr, "Error in line %d - unknown command\n", line_number);
        }
        else
        {
            fprintf(stderr, "Error in line %d - symbol cannot start with a non alphabetic character\n", line_number);
        }
        *syntax_errors = TRUE;
        return FALSE;
    }

    else if (begining_of_sentence)
    {
        if (length > 30)
        {
            fprintf(stderr, "Error in line %d - The word is too long to be a symbol\n", line_number);
            return FALSE;
        }
        if (current_word[length - 1] == ':')
        {
            current_word[length - 1] = '\0';
            if (is_saved_word(current_word, line_number, syntax_errors, print_err))
            {
                current_word[length - 1] = ':';
                return FALSE;
            }
            else
                return TRUE;
        }

        else
        {
            if (!is_saved_word(current_word, line_number, syntax_errors, 0))
            {
                fprintf(stderr, "Error in line %d - the symbol is missing ':'\n", line_number);
                *syntax_errors = 1;
            }
            return FALSE;
        }
    }

    else if (length <= 30 && is_alphanumeric(current_word))
        return !is_saved_word(current_word, line_number, syntax_errors, print_err);

    else
        return FALSE;
}

/* is_symbol_operand - operand which is not #<NUM> must start with an alphabetic char (either symbol or r<NUM>)
ONLY ERROR PRINT IS CHANGED IN THIS FUNC
receives: the current word to parse,
returns: 1 if it's a symbol, 0 if not. */
int is_symbol_operand(char *current_word, int line_number, int *syntax_errors, int begining_of_sentence, int print_err)
{
    size_t length = strlen(current_word);

    if (!isalpha(current_word[0]) && !is_command(current_word))
    {
        fprintf(stderr, "Error in line %d - operand cannot start with a non alphabetic character\n", line_number);
        *syntax_errors = TRUE;
        return FALSE;
    }

    else if (begining_of_sentence)
    {

        if (current_word[length - 1] == ':' && length <= 30)
        {
            current_word[length - 1] = '\0';
            return !is_saved_word(current_word, line_number, syntax_errors, print_err);
        }

        else
        {
            if (!is_saved_word(current_word, line_number, syntax_errors, 0))
            {
                fprintf(stderr, "Error in line %d - the symbol is missing ':'\n", line_number);
                *syntax_errors = 1;
            }
            return FALSE;
        }
    }

    else if (length <= 30 && is_alphanumeric(current_word))
        return !is_saved_word(current_word, line_number, syntax_errors, print_err);

    else
        return FALSE;
}

/* is_store_command -
 receives: the current word to parse,
 returns: 1 if the word is .data/.string, 0 if not. */
int is_store_command(char *current_word)
{

    if (strcmp_lower(current_word, ".data") == 0 || strcmp_lower(current_word, ".string") == 0)
    {
        memmove(current_word, current_word + 1, strlen(current_word) - 1); /* removes the "." from sentence. */
        current_word[strlen(current_word) - 1] = '\0';                     /* in order to strcmp to be accurate */
        return TRUE;
    }

    return FALSE;
}

/* detect_store_command -
  receives: the current sentence struct and the current word.
  If the current word matches to one of the store commands known, it is stored in the guidance_command variable of the
  sentence struct. */
void detect_store_command(sentence *parsed, char *current_word)
{
    if (strcmp_lower(current_word, "data") == 0)
        parsed->guidance_command = NUM;
    else if (strcmp_lower(current_word, "string") == 0)
        parsed->guidance_command = STRING;
    return;
}

/* is_extern_or_entry_command -
 receives: the current word to parse,
 returns: 1 if the word is .extern/.entry, 0 if not. */
int is_extern_or_entry_command(char *current_word)
{

    if (strcmp_lower(current_word, ".extern") == 0 || strcmp_lower(current_word, ".entry") == 0)
    {
        memmove(current_word, current_word + 1, strlen(current_word) - 1); /* removes '.' from the beginning */
        current_word[strlen(current_word) - 1] = '\0';                     /* in order to strcmp to be accurate */
        return TRUE;
    }

    return FALSE;
}

/* is_extern -
 receives: the current word after we detected it is "extern" or "entry",
 returns: 1 if it's "extern", 0 if not.
*/
int is_extern(char *current_word)
{
    return (strcmp_lower(current_word, "extern") == 0 ? TRUE : FALSE);
}

/* skip_spaces -
receives: line and last position.
returns the last postion where no empty spaces or \t are found. */
int skip_spaces(char *line, int last_position)
{
    while (*(line + last_position) == '\t' || *(line + last_position) == ' ')
        last_position++;
    return last_position;
}

/* get_next_member -
receives: a pointer to the temp member, the line to parse, line number, last position in line and if we are expecting to a comma 1, or 0 if we don't.
returns the new position and edits the temp_member array with the current number found.*/
int get_next_member(char *temp_member, char *line, int line_number, int last_position, int *syntax_errors, int *expecting_comma)
{
    int i = 0;
    int new_position = skip_spaces(line, last_position);
    int number_ended = 0; /*indication that no more digits are expected (for 3,4 4,5 example)*/

    while (line[new_position] != '\0' && line[new_position] != '\n' && line[new_position] != EOF && line[new_position] != '\t')
    {

        /*handle:
        2. add flag 'number ended' and prevent number after space
        3. check if finished with comma in outer func*/
        if (line[new_position] == '-' || line[new_position] == '+')
        {
            if (i == 0)
            {
                temp_member[i] = line[new_position];
                i++;
            }
            else
            {
                fprintf(stderr, "Error in line %d - valid number expected\n", line_number);
                *syntax_errors = 1;
            }
        }

        else if (isdigit(line[new_position]) && number_ended == 0)
        {
            *expecting_comma = 1; /* we can get a comma after getting at least 1 digit */
            temp_member[i] = line[new_position];
            i++;
        }
        else if (isdigit(line[new_position]) && number_ended == 1)
        {
            fprintf(stderr, "Error in line %d - expecting comma\n", line_number);
            *syntax_errors = 1;
            return new_position;
        }

        else if (line[new_position] == ',')
        {
            if (*expecting_comma == 1)
            {
                *expecting_comma = 0;
                new_position++;
                break;
            }
            else
            /* in case the "," comes after expecting_number = 1 it will be an error. */
            {
                fprintf(stderr, "Error in line %d - redundant ','.\n", line_number);
                *syntax_errors = 1;
            }
        }
        else if (line[new_position] == ' ')
        {
            number_ended = 1;
        }
        else
        {
            fprintf(stderr, "Error in line %d - only integers can come after .data store command\n", line_number);
            *syntax_errors = 1;
        }

        new_position++;
    }

    temp_member[i] = '\0';

    if (is_current_word_empty(temp_member) && line[new_position] != '\0' && line[new_position] != '\n' && line[new_position] != EOF)
        new_position = get_next_member(temp_member, line, line_number, new_position + 1, syntax_errors, expecting_comma);

    return new_position;
}

/* verify_and_save_numbers -
receives: the sentence struct were the parse is stored, the last position of the line, the line number and pointer to
syntax errors.
The function stores a number (positive or negative) into the data array of the sentence struct and the number of
numbers stored. */
void verify_and_save_numbers(sentence *parsed, char *line, int last_position, int line_number, int *syntax_errors)
{

    char temp_member[MAX_DIGITS_SIZE]; /* The range of numbers to be coded is from -512 to 511. The most big array can
                                       include "-512'\0'" and thus, we want to define MAX_DIGITS_SIZE to 5. */
    int expecting_comma = 0;
    int new_position = skip_spaces(line, last_position); /* initializes position */
    int j = 0;
    int number;

    while (line[new_position] != '\0' && line[new_position] != '\n' && line[new_position] != EOF && j < MAX_DATA_ARR_SIZE && line[new_position] != '\t')
    {

        new_position = get_next_member(temp_member, line, line_number, new_position, syntax_errors, &expecting_comma);
        number = atoi(temp_member);
        if (number > 511 || number < -512)
        {
            fprintf(stderr, "Error in line %d - the range of numbers that can be translated with assembler that works with 10 bits is from -512 to 511. Number %d cannot be stored.\n", line_number, number);
            *syntax_errors = 1;
        }
        else
        {
            parsed->data_arr[j] = number;
        }
        j++;
    }

    /*if expecting_comma == 0 it means that the last member encountered was a comma, or when we don't have any numbers.
    We reach to this "if" when the line ends and the data isn't completed */
    if (expecting_comma == 0)
    {
        fprintf(stderr, "Error in line %d - missing data parameters\n", line_number);
        *syntax_errors = 1;
    }

    parsed->data_arr_num_of_params = j;
    return;
}

/* verify_and_save_string -
   receives: the sentence struct, the line to parse, the last position in the line, the line number and a pointer to syntax errors.
   The function parses a string and saves it to sentence->string */
void verify_and_save_string(sentence *parsed, char *line, int last_position, int line_number, int *syntax_errors)
{
    char curr_char = line[last_position];
    int str_idx = 0;

    /*string should have the following format:
    1. space
    2. "
    3. charachters
    4. "
    5. end of line
    */

    /*1 - space*/
    if (curr_char != ' ')
    {
        fprintf(stderr, "Error in line %d - ' ' expected after '.string' cmd\n", line_number);
        *syntax_errors = 1;
    }
    if (curr_char == '\0' && curr_char == EOF && curr_char == '\n')
        return;

    /*skip spaces*/
    while (isspace(curr_char))
    {
        last_position++;
        curr_char = line[last_position];
    }
    /*2 - "*/
    if (curr_char != '\"')
    {
        fprintf(stderr, "Error in line %d - '\"' expected at the beginning of the string\n", line_number);
        *syntax_errors = 1;
    }

    if (curr_char == '\0' && curr_char == EOF && curr_char == '\n')
        return;

    last_position++;
    curr_char = line[last_position];

    /*3 - charachters*/
    while (curr_char != '\0' && curr_char != EOF && curr_char != '\n' && curr_char != '\"')
    {
        parsed->string[str_idx] = curr_char;
        str_idx++;
        last_position++;
        curr_char = line[last_position];
    }
    parsed->string[str_idx] = '\0';

    /*4 - "*/
    if (curr_char != '\"')
    {
        fprintf(stderr, "Error in line %d - '\"' expected at the end of the string\n", line_number);
        *syntax_errors = 1;
    }
    if (curr_char == '\0' && curr_char == EOF && curr_char == '\n')
        return;

    last_position++;
    curr_char = line[last_position];

    /*skip spaces*/
    while (curr_char == ' ')
    {
        last_position++;
        curr_char = line[last_position];
    }

    /*5 - end of line/file*/
    if (curr_char != '\0' && curr_char != EOF && curr_char != '\n')
    {
        fprintf(stderr, "Error in line %d -  unexpected characters after string ended\n", line_number);
        *syntax_errors = 1;
    }

    return;
}

/* parse_data_by_its_type -
   receives: the current sentence struct, the current word and the last position.
   it routes to the relevant function for parsing according the guidance command detected and stored in the guidance command
   variable in the sentence struct. */
void parse_data_by_its_type(sentence *parsed, char *line, int last_position, int line_number, int *syntax_errors)
{
    if (parsed->guidance_command == NUM)
        verify_and_save_numbers(parsed, line, last_position, line_number, syntax_errors);
    else if (parsed->guidance_command == STRING)
        verify_and_save_string(parsed, line, last_position, line_number, syntax_errors);
    return;
}

/* detect_opcode -
    receives: a pointer to the current word.
    returns: TRUE if the current word is an opcode, FALSE if it isn't. */
int detect_opcode(char *current_word)
{
    int opcode_idx = 0;
    while (opcode_idx < NUM_OF_OPCODES)
    {
        if (strcmp_lower(current_word, opcodes_table[opcode_idx].opcode) == 0)
            return TRUE;
        opcode_idx++;
    }
    return FALSE;
}

/* get_next_operand -
   receives: a pointer to current word, the line being parsed and the last position in the line.
   The function copies into current word the word that is supposed to be an operand. Verification for operand is doing in other functions.
   returns: the position where the function ended to read.*/
int get_next_operand(char *current_word, char *line, int last_position)

{
    int position = last_position;
    int i = 0;
    if (line[position] == ',')
    {
        current_word[0] = ',';
        current_word[1] = '\0';
        return position;
    }

    if (line[position] == '\n' || line[position] == EOF || line[position] == '\0')
    {
        current_word[i] = '\0';
        return position;
    }

    while (line[position] != ',' && line[position] != '\n' && line[position] != ' ' && line[position] != EOF && line[position] != '\0' && line[position] != '\t')
    {
        current_word[i] = line[position];
        i++;
        position++;
    }
    current_word[i] = '\0';

    if (is_current_word_empty(current_word))
        position = get_next_operand(current_word, line, position + 1);

    return position;
}

/* valid_reg_digit -
receives: a char.
returns: 1 if the char is a number between 0 to 7, 0 if not.
*/
int valid_reg_digit(char digit_char)
{
    if (digit_char < '0' || digit_char > '7')
        return 0;
    else
        return 1;
}

/* detect_operand - */
void detect_operand(char operand_position, sentence *parsed, char *temp_word, int line_number, int *syntax_errors, int *operands_in_sentence, int *temp_operand_type)
{

    if (temp_word[0] == '#')
    {
        *temp_operand_type = 0;
        *operands_in_sentence = *operands_in_sentence + 1;
        strncpy(temp_word, temp_word + 1, strlen(temp_word) - 1);
        temp_word[strlen(temp_word) - 1] = '\0';
        if (!atoi(temp_word) && strcmp(temp_word, "0") != 0)
        {
            fprintf(stderr, "Error in line %d - the value after an immediate operand must be a number\n", line_number);
            return;
        }
        else if (operand_position == 'a') /* if we are checking the operand in the 1st place after the opcode */
        {
            parsed->immediate_operand_a = atoi(temp_word);
            strcpy(parsed->source_operand_type, IMMEDIATE_OPERAND_TYPE);
        }
        else /* if we are checking the operand in the 2nd place after the opcode */
        {
            parsed->immediate_operand_b = atoi(temp_word);
            strcpy(parsed->dest_operand_type, IMMEDIATE_OPERAND_TYPE);
        }
        if ((atoi(temp_word) > 127 || atoi(temp_word) < -128))
        {
            fprintf(stderr, "Error in line %d - the value after an immediate operand must be a number\n", line_number);
            *syntax_errors = TRUE;
            return;
        }
    }

    else if (is_symbol_operand(temp_word, line_number, syntax_errors, FALSE, FALSE))
    {
        *temp_operand_type = 1;
        *operands_in_sentence = *operands_in_sentence + 1;
        if (operand_position == 'a')
        {
            strcpy(parsed->operand_1, temp_word);
            strcpy(parsed->source_operand_type, DIRECT_OPERAND_TYPE);
        }
        else
        {
            strcpy(parsed->operand_2, temp_word);
            strcpy(parsed->dest_operand_type, DIRECT_OPERAND_TYPE);
        }
    }
    else if (temp_word[0] == 'r' && valid_reg_digit(temp_word[1]))
    {
        if (strlen(temp_word) == 2)
        {
            *temp_operand_type = 3;
            *operands_in_sentence = *operands_in_sentence + 1;
            if (operand_position == 'a')
            {
                strcpy(parsed->operand_1, temp_word);
                strcpy(parsed->source_operand_type, REGISTER_OPERAND_TYPE);
            }
            else
            {
                strcpy(parsed->operand_2, temp_word);
                strcpy(parsed->dest_operand_type, REGISTER_OPERAND_TYPE);
            }
        }
    }

    return;
}
/* check_destination_address_type -
    receives: the current external iteration and the operand type.
    The function checks over the operands in the opcodes table that the type is one of it's destination address types.
    returns: true if supported, false if not.
    */
int check_destination_address_type(int ext_iteration, int op_type)
{
    int j;

    for (j = 0; opcodes_table[ext_iteration].destination_operand_types[j] != -1; j++)
    {
        if (opcodes_table[ext_iteration].destination_operand_types[j] == op_type)
            return TRUE;
    }

    return FALSE;
}

/* check_source_address_type -
    receives: the current external iteration and the operand type.
    The function checks over the operands in the opcodes table that the type is one of it's destination address types.
    returns: true if supported, false if not.*/

int check_source_address_type(int ext_iteration, int op_type)
{
    int j;

    for (j = 0; opcodes_table[ext_iteration].source_operand_types[j] != -1; j++)
    {
        if (opcodes_table[ext_iteration].source_operand_types[j] == op_type)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/* validate_operand_for_opcode -
    receives: the sentence struct, the type of operand in source, the type of operand in destination (-999 should be passed if empty),
    the line number and a pointer of type int to syntax error.
    The function validates the operand according the definitions in opcodes_table in structs.c.
    If the qty_of_ops is equal to the ammount of operands that the opcode supports,
    If op_a is equal to one of the supported operands types for source, if op_b is equal to one of the operands types for destination - the function returns TRUE.
    Else, it returns FALSE. */
int validate_operand_for_opcode(sentence *parsed, int op_a, int op_b, int qty_of_ops, int line_number, int *syntax_errors)
{
    int i;
    int operand_error = 0;

    for (i = 0; i < NUM_OF_OPCODES; i++)
    {
        if (strcmp_lower(parsed->opcode, opcodes_table[i].opcode) == 0)
        {
            if (opcodes_table[i].qty_of_supported_operands < qty_of_ops)
            {
                fprintf(stderr, "Error in line %d - too many operands for the opcode %s\n", line_number, parsed->opcode);
                *syntax_errors = 1;
                return FALSE;
            }
            else if (opcodes_table[i].qty_of_supported_operands > qty_of_ops)
            {
                fprintf(stderr, "Error in line %d - not enough valid operands\n", line_number);
                *syntax_errors = 1;
                return FALSE;
            }

            if (qty_of_ops == 1) /* if there is a single operand, it is categorized as destination operand */
            {
                if (check_destination_address_type(i, op_a) == FALSE)
                {
                    fprintf(stderr, "Error in line %d - the address type of the destination operand doesn't match to the opcode %s\n", line_number, parsed->opcode);
                    return FALSE;
                }
            }
            else if (qty_of_ops == 2)
            {
                if (check_source_address_type(i, op_a) == FALSE)
                {
                    fprintf(stderr, "Error in line %d - the address type of the source operand doesn't match to the opcode %s\n", line_number, parsed->opcode);
                    *syntax_errors = 1;
                    operand_error = 1;
                }

                if (check_destination_address_type(i, op_b) == FALSE)
                {
                    fprintf(stderr, "Error in line %d - the address type of the destination operand doesn't match to the opcode %s\n", line_number, parsed->opcode);
                    *syntax_errors = 1;
                    return FALSE;
                }
            }
        }
    }

    if (operand_error)
        return FALSE;
    else
        return TRUE;
}

int get_jump_operands(sentence *parsed, char *line_with_offset, int *syntax_errors)
{
    int i = 0, j = 0;
    char *op_a = (char *)calloc(strlen(line_with_offset) + 1, sizeof(char));
    char *op_b = (char *)calloc(strlen(line_with_offset) + 1, sizeof(char));
    for (; j < strlen(line_with_offset) && line_with_offset[i] != ',' && !isspace(line_with_offset[i]);)
    {
        op_a[j] = line_with_offset[i];
        i++;
        j++;
    }
    while (line_with_offset[i] != ',')
        i++;
    op_a[j] = '\0';
    i++;
    strcpy(parsed->jump_param_1, op_a);
    memmove(parsed->jump_param_1, op_a, strlen(op_a));
    j = 0;
    for (; j < strlen(line_with_offset) && line_with_offset[i] != ')' && !isspace(line_with_offset[i]);)
    {
        op_b[j] = line_with_offset[i];
        i++;
        j++;
    }
    while (line_with_offset[i] != ')')
    {
        i++;
    }
    op_b[j] = '\0';
    j++;
    i++;

    memmove(parsed->jump_param_2, op_b, strlen(op_b));

    if (atoi(parsed->jump_param_1) || strcmp(parsed->jump_param_1, "0") == 0)
    {
        convert_dec_to_x_bit_binary(atoi(parsed->jump_param_1), 3, parsed->source_operand_type);
    }
    else if (((parsed->jump_param_1[0] == '#') && (atoi(parsed->jump_param_1 + 1) || strcmp(parsed->jump_param_1 + 1, "0") == 0)))
    {
        convert_dec_to_x_bit_binary(atoi(parsed->jump_param_1 + 1), 3, parsed->source_operand_type);
    }

    if (atoi(parsed->jump_param_2) || strcmp(parsed->jump_param_2, "0") == 0)
    {
        convert_dec_to_x_bit_binary(atoi(parsed->jump_param_2), 3, parsed->dest_operand_type);
    }
    else if ((parsed->jump_param_2[0] == '#') && (atoi(parsed->jump_param_2 + 1) || strcmp(parsed->jump_param_2 + 1, "0") == 0))
    {
        convert_dec_to_x_bit_binary(atoi(parsed->jump_param_2 + 1), 3, parsed->dest_operand_type);
    }

    strcpy(parsed->dest_operand_type, JUMP_OPERAND_TYPE);

    free(op_a);
    free(op_b);
    return (i);
}




/* verify_operands -
  receives: the struct of sentence type, the last position in the line, the line number and a pointer to syntax errors.
  The function goes over the operands, check their type, validate they match the opcode and parse them into the
  sentence struct if legal. (Note: parsed->opcode holds the opcode already found).
  */
void verify_operands(sentence *parsed, char *line, int last_position, int line_number, int *syntax_errors)
{
    int operands_in_sentence = 0;
    int new_position;
    int temp_operand_type_a;
    int temp_operand_type_b;
    char temp_word[MAX_SYMBOL_SIZE];
    if ((strstr(line, "(") != NULL || strstr(line, ")") != NULL) && !parsed->is_jump)
    {
        printf("%s", line);
        fprintf(stderr, "Error in line %d - invalid instruction parentesis in none jump command\n", line_number);
        *syntax_errors = TRUE;
        return;
    }
    else if(parsed->is_jump) { /* @TODO Test this */
        int i = get_jump_operands(parsed, line, syntax_errors);
        if ((i + 1) == strlen(line)) {   
             printf("%d", i);
             return; 
             }
        else {
            fprintf(stderr, "Error in line %d - unexpected comma\n", line_number);
            *syntax_errors = TRUE;
            return;
        }
    }

    new_position = skip_spaces(line, last_position);
    new_position = get_next_operand(temp_word, line, new_position);

    if (strcmp(temp_word, ",") == 0) {
        fprintf(stderr, "Error in line %d - unexpected comma\n", line_number);
        *syntax_errors = TRUE;
        return;
    }

    if (is_current_word_empty(temp_word)) {
        validate_operand_for_opcode(parsed, -999, -999, operands_in_sentence, line_number, syntax_errors);
        return;
    }

    /*detect 1st operand */

    detect_operand('a', parsed, temp_word, line_number, syntax_errors, &operands_in_sentence, &temp_operand_type_a);

    /* finish pass over 1st operand */
    new_position = skip_spaces(line, new_position);
    if (line[new_position] == ',') {
        new_position = get_next_operand(temp_word, line, new_position + 1);
        if (strcmp(temp_word, ",") == 0) {
            fprintf(stderr, "Error in line %d - unexpected comma\n", line_number);
            *syntax_errors = TRUE;
            return;
        }

        if (is_current_word_empty(temp_word))
        {
            fprintf(stderr, "Error in line %d - missing operand after comma\n", line_number);
            return;
        }

        /* check for 2nd operand, increase operands_in_sentence if found other operand */
        detect_operand('b', parsed, temp_word, line_number, syntax_errors, &operands_in_sentence, &temp_operand_type_b);
    }

    if (line[new_position] != '\n' && line[new_position] != EOF && line[new_position] != '\0')
    {
        fprintf(stderr, "Error in line %d - end of line expected after matching number of operands\n", line_number);
        return;
    }

    parsed->num_of_operands = operands_in_sentence;

    if (operands_in_sentence == 1) /* if only a single operand was found, the operand is stored as destination */
    {

        strcpy(parsed->dest_operand_type, parsed->source_operand_type); /* 3 places so '\0' can be added - miunim */
        strcpy(parsed->source_operand_type, "");
        strcpy(parsed->operand_2, parsed->operand_1); /* for variables, registers, matrixes */
        strcpy(parsed->operand_1, "");
        parsed->immediate_operand_b = parsed->immediate_operand_a; /* when we have "#" */
        /* strcpy(parsed->matrix_row_operand_b, parsed->matrix_row_operand_a)
        strcpy(parsed->matrix_col_operand_a, ""); */
        if (validate_operand_for_opcode(parsed, temp_operand_type_a, -999, operands_in_sentence, line_number, syntax_errors))
            convert_dec_to_x_bit_binary(temp_operand_type_a, 3, parsed->dest_operand_type); /* VERIFY THE FUNCTION IS CORRECT */
        else
            return;
    }

    else if (operands_in_sentence == 2 && (validate_operand_for_opcode(parsed, temp_operand_type_a, temp_operand_type_b, operands_in_sentence, line_number, syntax_errors)))
    {
        convert_dec_to_x_bit_binary(temp_operand_type_a, 3, parsed->source_operand_type);
        convert_dec_to_x_bit_binary(temp_operand_type_b, 3, parsed->dest_operand_type);
    }

    return;
}

/* init_sentece -
   receives: the pointer to the new sentence declared.
   The function initiates all values to 0, empty or null in order to avoid garbage in fields. */
void init_sentence(sentence *parsed)
{
    int i;

    parsed->is_symbol = 0;
    parsed->is_action = 0;
    parsed->is_store_command = -1;
    parsed->guidance_command = -1;
    strcpy(parsed->symbol, "\0");
    strcpy(parsed->opcode, "\0");
    parsed->num_of_operands = -1;
    strcpy(parsed->source_operand_type, "\0");
    strcpy(parsed->dest_operand_type, "\0");
    strcpy(parsed->operand_1, "\0");
    strcpy(parsed->operand_2, "\0");
    parsed->immediate_operand_a = 0;
    parsed->immediate_operand_b = 0;
    strcpy(parsed->string, "\0");

    for (i = 0; i < MAX_DATA_ARR_SIZE; i++)
    {
        parsed->data_arr[i] = 0;
    }

    parsed->data_arr_num_of_params = 0;
    parsed->next = NULL;
}

/* line_is_empty -
   receives: the line to parse.
   returns true is the line is empty, or false if not. */
int line_is_empty(char *line)
{
    int i;
    i = skip_spaces(line, 0);

    return (line[i] == '\n' || line[i] == EOF);
}

/* right_symbol_identation -
    receives: the line to parse.
    returns true of it starts with an empty place. */
int right_symbol_identation(char *line)
{
    return !(line[0] == ' ' || line[0] == '\t');
}


int has_jump_command(char *line)
{
    return strstr(line, "jmp") != NULL || strstr(line, "bne") != NULL || strstr(line, "jsr") != NULL;
}


/*parse_sentence -
   receives: the line to be parsed and the current line number.
   The functions parses the line and populates the sentence structure (defined in data_structures.h) with the relevant fields.
   returns: a sentence structure. */
sentence *parse_sentence(char *line, int line_number, int *syntax_errors)
{

    int last_position;
    char current_word[80];
    int opc;

    sentence *parsed = (sentence *)malloc(sizeof(sentence));

    if (!parsed)
    {
        fprintf(stderr, "Memory allocation for new parsed sentence failed!");
        exit(1);
    }

    if (line_is_empty(line))
        return NULL;

    init_sentence(parsed);

    last_position = get_next_word(current_word, line, -1);
    if (is_symbol(current_word, line_number, syntax_errors, TRUE, TRUE))
    { /* if so, set is_symbol to 1, copy it to current_symbol and get the
next word into current_word */
        if (!right_symbol_identation(line))
        {
            fprintf(stderr, "Error in line %d - symbol doesn't start in 1st column\n", line_number);
            *syntax_errors = TRUE;
            return parsed;
        }
        parsed->is_symbol = 1;
        strcpy(parsed->symbol, current_word);
        last_position = get_next_word(current_word, line, last_position);
    }

    if (is_store_command(current_word))
    {
        parsed->is_store_command = 1;
        detect_store_command(parsed, current_word);
        parse_data_by_its_type(parsed, line, last_position, line_number, syntax_errors);
    }

    else if (is_extern_or_entry_command(current_word))
    {
        parsed->is_store_command = 0;
        if (strcmp_lower(current_word, "extern") == 0)
            parsed->guidance_command = EXTERN;
        else
            parsed->guidance_command = ENTRY;

        last_position = get_next_word(current_word, line, last_position);

        if (is_current_word_empty(current_word))
        {
            fprintf(stderr, "Error in line %d - missing symbol after guidance command\n", line_number);
            *syntax_errors = TRUE;
            return parsed;
        }
        else if (is_symbol(current_word, line_number, syntax_errors, FALSE, TRUE))
        {
            if (parsed->is_symbol == 1)
                fprintf(stderr, "Warning in line %d - the line has both symbol and extern declaration\n", line_number);
            strcpy(parsed->symbol, current_word);
        }
    }
    /* jump command flag*/
    parsed->is_jump = has_jump_command(current_word);

    /* if first word is not symbol||store_cmd||extern or if first is symbol and second is not store_cmd||extern:*/
    opc = detect_opcode(current_word);
    if (opc == TRUE)
    {
        parsed->is_action = 1;
        strcpy(parsed->opcode, current_word);
        verify_operands(parsed, line, last_position, line_number, syntax_errors);
    }

    return parsed;
}
