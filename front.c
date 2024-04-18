/*
=============================================================================
Title : front.c
Description : This program functions as the lexical analysis portion of a 
parser for the DCooke language specified in the Project 1 and 2 document.
Author : Joshua Ruegge
Date : 04/05/2024
Version : 1.1
Usage : Compile and run this program using the GNU C compiler. The parser
 accepts one mandatory command-line argument, which specifies the
name/path for the source code file. Ex: ./dcooke_parser test.dc
Notes : This program relies on the header files listed below, as well as 
the source file "parser.c".
C Version : GNU C complier 5.4.0
=============================================================================
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "front.h"
#include "parser.h"

/* Global Variable */
int nextToken;

/* Local Variables */
static int charClass;
char lexeme [100];
static char nextChar;
static int lexLen;
static FILE *in_fp;

/* Local Function declarations */
static void addChar();
static void getChar();
static void getNonBlank();

/* main driver function, including command line args */
int main(int argc, char* argv[]) 
{
    
    /* The program requires one argument specifying the source code path,
       so if no arguments (other than default path to program) are present, 
       display an error and stop */
    if(argc < 2) {
        printf("You must specify a path to a source code file as the first argument.\n");
        //exit code for Project 2 specifications
        return 2;
    }
    
    //Attempt to open the file at the specified path - display error if not present
    if((in_fp = fopen(argv[1], "r")) == NULL) {
        printf("The specified source code input file could not be located at the given path.\n");
        //exit code for Project 2 specifications
        return 3;
    }

    printf("DCooke Parser\n\n");
    
    //Parse the contents of the provided source file using the syntax analyzer.
    getChar();
    lex();
    program();

    //if execution has finished without errors and next token is end of file,
    //user file is free of syntax errors
    if(nextToken == EOF) {
        printf("Syntax Validated\n");
        return 0;
    } else {
        printf("Error encounter: The next lexeme was %s and the next token was %s\n", lexeme, codeToToken(nextToken));
        return 1;
    }
}

/* lookup - a function to lookup operators and parentheses and return the 
 * token */
static int lookup(char ch) {
    //no matter the character, the first step is to add it to lexeme[]
    addChar();
    switch (ch) {
        /* easy single-character lexemes - none of these can be the
           first character in a multi-character lexeme, so we can 
           safely classify them as their single-character token */
        case '(':
            nextToken = LEFT_PAREN;
            break;
        case ')':
            nextToken = RIGHT_PAREN;
            break;
        case '=':
            nextToken = EQUAL_OP;
            break;
        case '*':
            nextToken = MULT_OP;
            break;
        case '/':
            nextToken = DIV_OP;
            break;
        case '%':
            nextToken = MOD_OP;
            break;
        case ';':
            nextToken = SEMICOLON;
            break;
        /* Each of these characters could be either a single-character
           lexeme, or the start of a multi-character lexeme, so we have to
           check all possibilities for each. */
        case ':':
            //This could be either : or :=, so check next char
            getChar();
            if(nextChar == '=') {
                //add second character to lexeme and set nextToken to multi-character token
                addChar();
                nextToken = ASSIGN_OP;
            } else {
                /* because this turned out to be a single-character lexeme,
                   we need to back the file pointer up by one character
                   so that the getChar() call that executes immediately after
                   lookup() finishes will leave the correct char in nextChar */
                nextToken = COLON;
                fseek(in_fp, -1, SEEK_CUR);
            }
            break;
        /* each of the below lexemes follows a similar format as above - 
           check for multi-character lexeme, and if not present, 
           back file pointer up as necessary. */
        case '<':
            getChar();
            if(nextChar == '=') {
                addChar();
                nextToken = LEQUAL_OP;
            } else if (nextChar == '>') {
                addChar();
                nextToken = NEQUAL_OP;
            } else {
                nextToken = LESSER_OP;
                fseek(in_fp, -1, SEEK_CUR);
            }
            break;
        case '>':
            getChar();
            if(nextChar == '=') {
                addChar();
                nextToken = GEQUAL_OP;
            } else {
                nextToken = GREATER_OP;
                fseek(in_fp, -1, SEEK_CUR);
            }
            break;
        case '+':
            getChar();
            if(nextChar == '+') {
                addChar();
                nextToken = INC_OP;
            } else {
                nextToken = ADD_OP;
                fseek(in_fp, -1, SEEK_CUR);
            }
            break;
        case '-':
            getChar();
            if(nextChar == '-') {
                addChar();
                nextToken = DEC_OP;
            } else {
                nextToken = SUB_OP;
                fseek(in_fp, -1, SEEK_CUR);
            }
            break;
        /* The call to lookup() only takes place if the character was present in VALID_SYMBOLS,
           and all the possible characters in VALID_SYMBOLS are handled above. Therefore, if
           we get to this default case, something has gone disastrously wrong - so, mark next
           token as EOF in order to stop analysis. */
        default:
            nextToken = EOF;
            break;
    }
    return nextToken;
}

/* addChar - a function to add nextChar to lexeme */
static void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    } else {
        printf("Error - lexeme is too long \n");
    }
}

/* getChar - a function to get the next character of input and determine its 
   character class */
static void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        /* use strchr to check if character is one of the valid symbols for the language
           this is done by checking if it has a non-null location in the macro string 
           VALID_SYMBOLS, which contains all the valid nonalphanumeric characters */
        else if (strchr(VALID_SYMBOLS, nextChar) != NULL)
            charClass = SYMBOL;
        else charClass = UNKNOWN;
    } else {
        charClass = EOF;
    }
}

/* getNonBlank - a function to call getChar until it returns a non-whitespace 
   character */
static void getNonBlank() {
    while (isspace(nextChar)) getChar();
}

/* codeToToken - a function that takes a token code and returns the
   corresponding token name as a string.
   Ex. the token INT_LIT is defined via macro as a code value of 10,
   so a call of codeToToken(10) will return the string "INT_LIT" */
const char* codeToToken(int code) {
    switch(code) {
        case 10:
	    return "INT_LIT";

        case 11:
            return "IDENT";

        case 13:
            return "KEY_READ";

        case 14:
            return "KEY_WRITE";

        case 15:
            return "KEY_IF";

        case 16:
            return "KEY_FI";

        case 17:
            return "KEY_ELSE";

        case 18:
            return "KEY_THEN";

        case 21:
            return "ADD_OP";

        case 22:
            return "SUB_OP";

        case 23:
            return "MULT_OP";

        case 24:
            return "DIV_OP";

        case 25:
            return "MOD_OP";

        case 26:
            return "INC_OP";

        case 27:
            return "DEC_OP";

        case 30:
            return "LESSER_OP";

        case 31:
            return "GREATER_OP";

        case 32:
            return "EQUAL_OP";

        case 33:
            return "NEQUAL_OP";

        case 34:
            return "LEQUAL_OP";

        case 35:
            return "GEQUAL_OP";

        case 40:
            return "ASSIGN_OP";

        case 41:
            return "LEFT_PAREN";

        case 42:
            return "RIGHT_PAREN";

        case 43:
            return "SEMICOLON";

        case 44:
            return "COLON";
        
        default:
            return "UNKNOWN";
    }
}

/* lex - a simple lexical analyzer to identify the token of the next lexeme in the file */
int lex() {
    lexLen = 0;
    getNonBlank();

    switch (charClass) {
        /* Parse identifiers */
        case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }
            /* use strcmp to compare the letter lexeme to various macros
               in order to determine whether it is a keyword or identifier
               note that strcmp returns a value of 0 if the strings are
               equal, so !strcmp() is necessary to return true for equality*/
            if(!strcmp(lexeme, STRING_READ))
                nextToken = KEY_READ;
            else if (!strcmp(lexeme, STRING_WRITE))
                nextToken = KEY_WRITE;
            else if (!strcmp(lexeme, STRING_IF))
                nextToken = KEY_IF;
            else if (!strcmp(lexeme, STRING_FI))
                nextToken = KEY_FI;
            else if (!strcmp(lexeme, STRING_ELSE))
                nextToken = KEY_ELSE;
            else if (!strcmp(lexeme, STRING_THEN))
                nextToken = KEY_THEN;
            else 
                nextToken = IDENT;
            break;

        /* Parse integer literals */
        case DIGIT:
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
            nextToken = INT_LIT;
            break;

        /* Parentheses and operators */
        case SYMBOL:
            lookup(nextChar);
            getChar();
            break;

        /* Unknown lexeme */
        case UNKNOWN:
            addChar();            
            nextToken = UNKNOWN;
            getChar();
            break;

        /* EOF */
        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            //return early to ensure EOF lexeme is not printed
            return nextToken;
    } 
    
    //Leftover/debug print statement from Project 1 specifications
    //printf("%s\t%s\n", lexeme, codeToToken(nextToken));
    return nextToken;
}

