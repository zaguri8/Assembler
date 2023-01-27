/*
//  converters.c
//  Assembler
//
//  Created by Nadav Avnon on 01/01/2023.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "converters.h"

#define WORKING_BASE 2

typedef struct s2_special_entry {
    int digit;
    char character[2];
}s2_special_entry;

typedef struct binary_to_s2_special {
    char binary[3];
    char character[2];
} binary_to_s2_special;

s2_special_entry s2t [] = {
        {0, "."},
        {1, "/"},
};



int special_2_table_size = sizeof(s2t)/sizeof(s2t[0]);

/* Converts a digit to a special 2 character */
char * digit_to_char_special_2(int digit){
    int i;
    for (i=0; i < special_2_table_size; i++){
        if (digit == s2t[i].digit) {
            return s2t[i].character;
        }
    }
    fprintf(stderr, "Error: digit provided is not in 2 special table\n");
    return NULL;
}

/* Converts a decimal number to any other base. */
int convert_dec_to_another_base(int number,int base){
    if(number == 0 || base==10)
        return number;
    return (number % base) + 10*convert_dec_to_another_base(number / base, base);
}

/* Converts a decimal number to base 2 special. */
void convert_dec_to_base_2_special(int num, char * result) {
    convert_base_2_to_base_2_special(convert_dec_to_another_base(num, WORKING_BASE), result);
}

/* Converts a decimal number to a binary number with padded zeroes up to x bits.
* When calling the function array of chars sized number of bits + 1 (for '\0') should be provided (char * result) */
void convert_dec_to_x_bit_binary(int num, int bits, char * result){
    int i = bits - 1;
    int binary_num = convert_dec_to_another_base(num, 2);

    result[i] = '\0';
    i--;
    do {
        result[i] = binary_num % 10 == 0 ? '0' : '1';
        binary_num = binary_num / 10;
        i--;
    }
    while (binary_num);

    for (; i >= 0; i--) { /* pad with zeros */
        result[i] = '0';
    }
    /* perform 2's complement if it's a negative number */
    if (num < 0) {
        int i = bits - 2;
        int first_one_encountered = 0;
        for (; i >= 0; i--) {
            if (first_one_encountered == 0) {
                if (result[i] == '1') {
                    first_one_encountered = 1;
                    continue;
                }
            }
            else {
                result[i] = result[i] == '1' ? '0' : '1';
            }
        }
    }
}

/* Coverts an alphabetic char into its binary value using 13 bits (with zero padding)
* When calling the function array of chars sized number of bits + 1 (for '\0') should be provided (char * result) */
void convert_ascii_value_to_10_bit_binary(char c, char * result){
    convert_dec_to_x_bit_binary(c, 14, result);
}

/* Helper function to convert_base_2_to_base_2_special. */
void convert_base_2_to_base_2_special_internal(int num, char * result){
    if (num < 10 && num >= 0) {
        strcpy(result, digit_to_char_special_2(num));
        return;
    }
    convert_base_2_to_base_2_special_internal(num / 10, result);
    strcat(result, digit_to_char_special_2(num % 10));
}

/* converts a number in base 2 to a number in base 2 special. */
void convert_base_2_to_base_2_special(int num, char * result){
    convert_base_2_to_base_2_special_internal(num, result);
    strcat(result, "\0");
}

void convert_binary_to_base_2_special(char binary_address[14], char * result) {
    int i, k;
    char one_char[2];
    for (i = 0; i < 14; i+=1 ) {
        strncpy(one_char, binary_address+i, 1);
        one_char[1] = '\0';
        for (k=0; k < special_2_table_size; k++) {
            if (atoi(one_char) == s2t[k].digit) {
                if (i == 0) {
                    strcpy(result, s2t[k].character);
                }
                else {
                    strcat(result, s2t[k].character);
                }
                break;
            }
        }
    }
    strcat(result, "\0");
}
