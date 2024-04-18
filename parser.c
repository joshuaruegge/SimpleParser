/*
=============================================================================
Title : parser.c
Description : This program functions as the syntax analysis portion of a 
parser for the DCooke language, as specified in the Project 2 document.
Author : Joshua Ruegge
Date : 04/05/2024
Version : 1.0
Usage : Compile and run this program using the GNU C compiler. The parser
accepts one mandatory command-line argument, which specifies the name/path
for the source code file. Ex: ./dcooke_parser test.dc
Notes : This program relies on the header files listed below, as well as the
source file "front.c".
C Version : GNU C compiler 5.4.0 
=============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "front.h"

static void error();

 /* program - root function representing the following root rule of the BNF tree:
  * 
  * P :== S
  *
  * the program (P) rule can only go directly to statement, so the only purpose this function
  * serves is as a designated entry point to be called by the analyzer initially.
  */
void program() {
    //only one path - call statement
    statement();
}

/* statement - central function handling the S rule of the BNF tree:
 *
 * S ::= V:=E | read(V) | write(E) | if C: then S U fi | S;S
 *
 * Note that the simple sub-rule U :== else S | epsilon is handled here as well
 */
 void statement() {
     // V:=E rule (desired token sequence: IDENT, ASSIGN_OP, call expression())
     if(nextToken == IDENT) {
         lex();
         if(nextToken == ASSIGN_OP) {
             lex();
             expression();
         } else error();
     // read(V) rule (desired token sequence: KEY_READ, LEFT_PAREN, IDENT, RIGHT_PAREN)
     } else if (nextToken == KEY_READ) {
         lex();
         if(nextToken == LEFT_PAREN) {
             lex();
             if(nextToken == IDENT) {
                 lex();
                 if(nextToken == RIGHT_PAREN) {
                     lex();
                 } else error();
             } else error();
         } else error();
     // write(E) rule (desired token sequence: KEY_WRITE, LEFT_PAREN, call expression(), RIGHT_PAREN)
     } else if(nextToken == KEY_WRITE) {
         lex();
         if(nextToken == LEFT_PAREN) {
             lex();
             expression();
             if(nextToken == RIGHT_PAREN) {
                     lex();
             } else error();
         } else error();
     // if rule (desired token sequence: KEY_IF, call comparison(), COLON, KEY_THEN, 
     //call statement(), optionally handle else, KEY_FI)
     } else if(nextToken == KEY_IF) {
         lex();
         comparison();
         //lex(); any submethod should assume next token is valid to start,
         //and leave necessary next token on completion?
         if(nextToken == COLON) {
             lex();
             if(nextToken == KEY_THEN) {
                 lex();
                 statement();
                 //handle optional else
                 if(nextToken == KEY_ELSE) {
                     lex();
                     statement();
                 }
                 if(nextToken == KEY_FI) {
                     lex();
                 } else error();
             } else error();
         } else error();
     } else error();
     //optional continuation via semicolon
     if(nextToken == SEMICOLON) {
         lex();
         statement();
     }
 }

 /* comparison - function handling the C rule of the BNF tree:
  *
  * C ::= ( C ) | E < E | E > E | E = E | E <> E | E <= E | E >= E
  */
void comparison() {
    //(C) rule (desired token sequence: LEFT_PAREN, call comparision(), RIGHT_PAREN)
    if(nextToken == LEFT_PAREN) {
        lex();
        comparison();
        if(nextToken == RIGHT_PAREN) {
            lex();
        } else error();
    //all other rules begin with expression (desired token sequence: call expression(), 
    //one of many comparison ops, call expression())
    } else {
        expression();
        if(nextToken == LESSER_OP || nextToken == GREATER_OP || nextToken == EQUAL_OP || nextToken == NEQUAL_OP || nextToken == LEQUAL_OP || nextToken == GEQUAL_OP) {
            lex();
            expression();
        } else error();
    }
}

/* expression - function handling the E rule of the BNF tree:
 *
 * E ::= T | E + T | E - T
 *
 * note that this rule suggests that any expression will consist of at least one term, followed
 * optionally by a + or - and another expression
 */
void expression() {
    //expression must start with at least one term
    term();
    //optional continuation
    if(nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        expression();
    }
}

/* term - function handling the T rule of the BNF tree:
 *
 * T ::= F | T * F | T / F | T % F
 *
 * note that as with expression() above, this rule suggests that each term consists
 * of at least one factor, followed optionally by a *, /, or % and then another term
 */
void term() {
    //term must start with a factor
    factor();
    //optional continuation
    if(nextToken == MULT_OP || nextToken == DIV_OP || nextToken == MOD_OP) {
        lex();
        term();
    }
}

/* factor - function handling the F rule of the BNF tree:
 *
 * F ::= (E) | O | N | V
 *
 * note that this function also handles the simple subrule O = ++V | --V.
 */
void factor() {
    //(E) rule (desired token sequence: LEFT_PAREN, call expression(), RIGHT_PAREN)
    if(nextToken == LEFT_PAREN) {
        lex();
        expression();
        if(nextToken == RIGHT_PAREN) {
            lex();
        } else error();
    //O rule (desired token sequence: INC_OP or DEC_OP, IDENT)
    } else if(nextToken == INC_OP || nextToken == DEC_OP) {
        lex();
        if(nextToken == IDENT) {
            lex();
        } else error();
    //V or N rule (desired token sequence: IDENT or INT_LIT)
    } else if(nextToken == IDENT || nextToken == INT_LIT) {
        lex();
    } else error();
}

/* error - function handling when an error is encountered during the syntax analysis process.
 * Prints out "Error encounter: The next lexeme was <lexeme> and the next token was <token>�
 * using information fetched from front.
 * Once this is done, exit with an exit code of 1 as mentioned in project requirements.
 */
static void error() 
{
    printf("Error encounter: The next lexeme was %s and the next token was %s\n", lexeme, codeToToken(nextToken));
    exit(1);
}
