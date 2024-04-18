#ifndef FRONT_H
#define FRONT_H

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define SYMBOL 2
#define UNKNOWN 99

/* Token codes */
//Integer literals and identifiers
#define INT_LIT 10
#define IDENT 11
//Keywords
#define KEY_READ 13
#define KEY_WRITE 14
#define KEY_IF 15
#define KEY_FI 16
#define KEY_ELSE 17
#define KEY_THEN 18
//Arithmetic ops
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define MOD_OP 25
#define INC_OP 26
#define DEC_OP 27
//Comparison ops
#define LESSER_OP 30
#define GREATER_OP 31
#define EQUAL_OP 32
#define NEQUAL_OP 33
#define LEQUAL_OP 34
#define GEQUAL_OP 35
//Grouping and misc.
#define ASSIGN_OP 40
#define LEFT_PAREN 41
#define RIGHT_PAREN 42
#define SEMICOLON 43
#define COLON 44

/* Keyword string constants */
#define STRING_READ "read"
#define STRING_WRITE "write"
#define STRING_IF "if"
#define STRING_FI "fi"
#define STRING_ELSE "else"
#define STRING_THEN "then"

//All valid non-alphanumeric symbols in language
#define VALID_SYMBOLS ";:=<>()+-*/%"

int lex();
const char* codeToToken(int code);

#endif
