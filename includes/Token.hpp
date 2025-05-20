// Created by Anei Markovič 19.05.2024
/*
    Razred, ki shranjuje tokene in njihove informacije
*/
#ifndef TOKEN_H
#define TOKEN_H
#include <iostream>
#include <string>

class Token
{
private:
    std::string lexem;
    int col;
    int row;
    int token;
    bool eof;

public:
    Token(const std::string &aLexem, int aColumn, int aRow, int aToken, bool aEof);

    Token();

    const std::string getLexem() const;

    const int getRow() const;

    const int getCol() const;

    const int getToken() const;

    const bool isEof() const;

    friend std::ostream &operator<<(std::ostream &out, const Token &aToken);
};

#endif