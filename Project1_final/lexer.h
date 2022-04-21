/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0,
    MAIN, PROC,
    ENDPROC, INPUT, OUTPUT , DO , EQUAL, NUM, ID, 
    SEMICOLON, PLUS , MINUS, MULT, DIV, ERROR
    } TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    Token peek(int);
    LexicalAnalyzer();

  private:
    std::vector<Token> tokenList;
    int line_no;
    int index;
    Token tmp;
    InputBuffer input;

    Token GetTokenMain();
    bool SkipSpace();
    int FindKeywordIndex(std::string);
    Token ScanNumber();
    Token ScanIdOrKeyword();
};

#endif  //__LEXER__H__
