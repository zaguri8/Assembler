all: my_assembler
my_assembler: main.c assembler.c initial_entry.c secondary_entry.c pre_assembler.c pre_assembler.h assembler.h converters.c converters.h structs.c structs.h parser.c parser.h
	gcc -g -Wall -ansi -pedantic main.c assembler.c pre_assembler.c initial_entry.c secondary_entry.c converters.c structs.c parser.c -o my_assembler -lm
clean: rm  my_assembler
