#pragma once

#include "Lexer.hpp"

/*
	Created by Žan Misja 26.05.2024
	Razred preveri ali je datoteka veljana glede na gramatiko BNF
*/

class Parser
{
private:
	Lexer m_lexer;
    Token m_currentToken;

    bool match(int expectedType);
    bool tryToConsume(int expectedType);
    void error(std::string errorMessage);

    bool parseBegin();

    bool parseBeginPrime();
    bool parseNonAssignCommand();
    bool parseStart();
    bool parseMinetype();

    bool parseLandelements();
    bool parseLandelementsPrime(); 
    bool parseUndergroundelements();
    bool parseUndergroundelementsPrime();
    bool parseSeaelements();
    bool parseSeaelementsPrime();

    bool parseLandelementsElement();
    bool parseLandelementsCurlyChoice();
    bool parsePathVsebina();
    bool parseRiverVsebina();
    bool parseLandelementsBracketChoice();
    bool parseStripVsebina();
    bool parseBenchVsebina();
    bool parseSpoilpitVsebina();
    bool parsePlantVsebina();
    bool parseWellVsebina();

    bool parseUndergroundelementsElement();
    bool parseUndergroundelementsCurlyChoice();
    bool parseElevatorVsebina();
    bool parseOrebedVsebina();
    bool parseUndergroundelementsBracketChoice();
    bool parseTunnelVsebina();
    bool parseShaftVsebina();

    bool parseSeaelementsElement();
    bool parseSeaelementsCurlyChoice();
    bool parsePlatformVsebina();
    bool parsePumpVsebina();
    bool parseRiserVsebina();

    bool parseLines();
    bool parseLinesContent();
    bool parseLinesItemContentAfterParen();
    bool parseLinesBendOpt();

    bool parseArbitarraypolygon();
    bool parsePolyline();
    bool parseLine();
    bool parseBox();
    bool parsePoints();
    bool parsePointsPrime();
    bool parsePoint();
    bool parseBend();
    bool parseCirc();

    bool parseExpr();
    bool parseBitwise();
    bool parseBitwisePrime();
    bool parseAdditive();
    bool parseAdditivePrime();
    bool parseMultiplicative();
    bool parseMultiplicativePrime();
    bool parseUnary();
    bool parsePrimary();

    bool parseCommandSet();
    bool parseCommandSetPrime();
    bool parseCommand();
    bool parseLoop();
    bool parseAssign();
    bool parseAssignDesno(); 
    bool parsePrint();

    bool parseFunction();
    bool parseAtribute();
    bool parseAtributePrime();
    bool parseFunctionCall();

public:
    Parser(Lexer lexer) : m_lexer(lexer) { m_currentToken = m_lexer.nextToken(); }

    bool parseFile(const std::string& filePath);
};