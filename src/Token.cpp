// Created by Anei Markovič 19.05.2024
#include "Token.hpp"

Token::Token(const std::string &aLexem, int aColumn, int aRow, int aToken, bool aEof) : lexem(aLexem), col(aColumn), row(aRow), token(aToken), eof(aEof) {}

Token::Token() : lexem("") {}

// metoda, ki pridobi lexem tokena
const std::string Token::getLexem() const
{
    return lexem;
}

// Metoda, ki dobi vrstico
const int Token::getRow() const
{
    return row;
}

// Metoda, ki dobi stolpec
const int Token::getCol() const
{
    return col;
}

// Metoda, ki dobi token
const int Token::getToken() const
{
    return token;
}

// Metoda, ki preveri ali je konec datoteke
const bool Token::isEof() const
{
    return eof;
}

// Preobremenitev operatorja << za izpis objekta Token v izhodni tok
std::ostream &operator<<(std::ostream &out, const Token &aToken)
{
    out << "'" << aToken.getLexem() << "' " << aToken.getToken() << " (" << aToken.getRow() << ", " << aToken.getCol() << ") " << (aToken.isEof() ? "true" : "false");
    return out;
}