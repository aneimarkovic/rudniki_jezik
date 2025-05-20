// Created by Anei Markovič 19.05.2024
/*
    Razred vzame input iz datoteke in pridobi tokene, ki jih pošlje dalje parserju
*/
#ifndef LEXER_H
#define LEXER_H
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include "Token.hpp"

class Lexer
{
private:
    std::istream *input;
    Token lastToken;
    int row;
    int col;
    std::map<std::string, double> dictionary;

    // const static int maxState = 41;
    const static int maxState = 226;
    const static int startState = 0;
    const static int noEdge = -1;

    int automata[maxState + 1][256];
    int finite[maxState + 1];
    void initAutomata();

protected:
    int getNextState(int aState, int aChar) const;

    bool isFiniteState(int aState) const;

    int getFiniteState(int aState) const;

private:
    int peek();

    int read();

    Token nextTokenImp();

public:
    const static int tLexError = -1;
    const static int tIgnore = 0;
    const static int tInteger = 1;
    const static int tHashTag = 2; // Ignore
    const static int tHex = 3;
    const static int tVarLetter = 4;
    const static int tVarDigit = 5;
    const static int tBackSlash = 6; // Ignore
    const static int tPlus = 7;
    const static int tMinus = 8;
    const static int tTimes = 9;
    const static int tSlash = 10;
    const static int tAND = 11;
    const static int tOR = 12;
    const static int tBracketOpen = 13;
    const static int tBracketClosed = 14;
    const static int tOnlyTimes = 18;
    const static int tSemiColon = 15;
    const static int tEquals = 17;
    const static int tBegin = 26;
    const static int tFor = 21;
    const static int tEnd = 29;
    const static int tConsole = 36;
    const static int tOnlyPlus = 37;

    const static int tBend = 43;
    const static int tBench = 45;
    const static int tBox = 47;

    const static int tCall = 50;
    const static int tCircle = 55;

    const static int tElevator = 62;

    const static int tDeepsea = 69;

    const static int tHuman = 74; 

    const static int tInplace = 81;
    const static int tInsitu = 85;
    const static int tInjection = 92;

    const static int tLine = 96;

    const static int tMonitoring = 106;

    const static int tOre = 109;
    const static int tOreBed = 112;

    const static int tPath = 116;
    const static int tPlatform = 123;
    const static int tPolygon = 129;
    const static int tPolyline = 133;
    const static int tPoint = 136;
    const static int tPump = 139;
    const static int tProcessing = 148;

    const static int tRiver = 153;
    const static int tRiser = 156;
    const static int tRecovery = 163;
    const static int tReturn = 167;

    const static int tStorage = 174;
    const static int tStrip = 177;
    const static int tSurface = 183;
    const static int tShaft = 187;
    const static int tSpoilpit = 194;

    const static int tTunnel = 200;

    const static int tUnderground = 211;

    const static int tWashing = 218;
    const static int tWell = 221;

    const static int tComma = 222;
    const static int tSquareBracketOpen = 223;
    const static int tSquareBracketClosed = 224;
    const static int tCurlyBracketOpen = 225;
    const static int tCurlyBracketClosed = 226;


    bool eof();
    Lexer(std::istream *aInput);
    Token nextToken();
    Token currentToken();
    void printAllTerminals();

    void insertVariable(const std::string &name, double value);
    double getVarValue(std::string &name);
    void updateVarValue(std::string &name, double value);
    std::streampos getPosition();
    void changePosition(std::streampos &pos);
};

#endif