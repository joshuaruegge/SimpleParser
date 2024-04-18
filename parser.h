#ifndef PARSER_H
#define PARSER_H

void program();
void statement();
void comparison();
void expression();
void term();
void factor();

extern int nextToken;
extern char lexeme[100];
#endif
