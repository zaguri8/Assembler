/*
//  main.c
//  Assembler
//
//  Created by Nadav Avnon on 12/01/2023.
*/

#include <stdlib.h>
#include "assembler.h"
#include "pre_assembler.h"

#define ASSEMBLY_SRC_FILE_EXTENSION ".as"

int main(int argc, char * argv[]) {
    FILE * fd;
    int i;
    
    /* verify command line arguments */
    if (argc == 1) {
        fprintf(stderr, "ERROR: You must enter at least 1 assembly (%s) input file name as a command line argument\n", ASSEMBLY_SRC_FILE_EXTENSION);
        exit(2);
    }

    /* verify files exist, files extension, file length and read permissions */
    for (i = 1; i < argc; i++) {
        char * filename = argv[i];

        /* check filename length and extension */
        if (strlen(filename) < 4) {
            fprintf(stderr, "ERROR: file %s is too short.\n", filename);
            exit(2);
        }
        else { /* check file extension */
            char * file_extension = &filename[strlen(filename)-3];
            if (strcmp(file_extension, ASSEMBLY_SRC_FILE_EXTENSION) != 0) {
                fprintf(stderr, "ERROR: the file %s has wrong extension (%s)\n", filename, ASSEMBLY_SRC_FILE_EXTENSION);
                exit(2);
            }
        }
        /* check files exist and permissions */
        if (!(fd = fopen(filename, "r"))) {
            fprintf(stderr, "ERROR: There is no such file %s or you don't have read permissions on it\n", filename);
            exit(2);
        }
    }

    for (i = 1; i < argc; i++) {
        char * filename = argv[i];
        fd = fopen(argv[i], "r");
        printf("-----------------------------------------------------------------\n");
        printf("INFO: Running pre assembler on file %s\n", filename);
        if(PreAssembler_Execute(filename)) {
            printf("INFO: Finished running pre assembler on file %s\n", filename);
            printf("-----------------------------------------------------------------\n");
        }
        else {
            fprintf(stderr, "ERROR: Errors found in pre assembler. assembler will not be run.\n");
            return 1;
        }
        printf("INFO: Running assembler on file %s\n", filename);
        if (ExecuteInitialEntry(fd)) {
            printf("INFO: First pass run completed successfully on file %s\n", filename);
            printf("INFO: Running secondary entry on file %s\n", filename);
            ExecuteSecondEntry(filename);
            printf("INFO: Finished running assembler on file %s\n", filename);
            printf("-----------------------------------------------------------------\n");
        }
        else {
            fprintf(stderr, "ERROR: Errors found in initial entry. secondary entry will not be run.\n");
        }
        fclose(fd);
    }

    return 0;
}
