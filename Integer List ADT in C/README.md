An integer list ADT built in C and used to alphabetize the lines in a file.
Underlying data structure for the List ADT is a doubly linked list using sentinels for head and tail.

Tour of the files: 
List.h        Header file of list ADT. Specify function prototypes
List.c        Source file of list ADT. Implementation of the functions specified in List.h
ListClient.c  Test client for list ADT
Lex.c         Main program of this project. Take two command line arguments namingly input file and output file. Read lines from input                   files. Sort them in lexicographic order and store the index in the list ADT. Print lines in sorted order to the output file
Makefile      Run make to build the executable binary file Lex
tests/        Testing input files and sample output files





