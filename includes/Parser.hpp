#pragma once

#include "Lexer.hpp"
#include "Ast.hpp"
#include <memory>

/*
	Created by Žan Misja 26.05.2024
	Razred preveri ali je datoteka veljana glede na gramatiko BNF
*/

class Parser {
private:
    Lexer m_lexer;
    Token m_currentToken;
    std::unique_ptr<ProgramNode> m_programNode;

    Token currentTokenForAst() { return m_currentToken; }
    Token previousTokenForAst(const Token& consumedToken) { return consumedToken; }


    bool match(int expectedType);
    bool tryToConsume(int expectedType, Token* consumedToken = nullptr);
    void error(std::string errorMessage);

    std::unique_ptr<StatementNode> parseBegin();
    std::unique_ptr<BeginPrimeBodyNode> parseBeginPrime();
    std::unique_ptr<StatementNode> parseNonAssignCommand();
    std::unique_ptr<StartNode> parseStart();
    std::unique_ptr<MinetypeNode> parseMinetype();

    std::unique_ptr<LandElementsNode> parseLandelements();
    bool parseLandelementsPrime(LandElementsNode* ownerNode);
    std::unique_ptr<UndergroundElementsNode> parseUndergroundelements();
    bool parseUndergroundelementsPrime(UndergroundElementsNode* ownerNode);
    std::unique_ptr<SeaElementsNode> parseSeaelements();
    bool parseSeaelementsPrime(SeaElementsNode* ownerNode);

    std::unique_ptr<LandElementNode> parseLandelementsElement();
    std::unique_ptr<LandElementCurlyNode> parseLandelementsCurlyChoice();
    std::unique_ptr<PathVsebinaNode> parsePathVsebina();
    std::unique_ptr<RiverVsebinaNode> parseRiverVsebina();
    std::unique_ptr<LandElementBracketNode> parseLandelementsBracketChoice();
    std::unique_ptr<StripVsebinaNode> parseStripVsebina();
    std::unique_ptr<BenchVsebinaNode> parseBenchVsebina();
    std::unique_ptr<SpoilpitVsebinaNode> parseSpoilpitVsebina();
    std::unique_ptr<PlantVsebinaNode> parsePlantVsebina();
    std::unique_ptr<WellVsebinaNode> parseWellVsebina();

    std::unique_ptr<UndergroundElementNode> parseUndergroundelementsElement();
    std::unique_ptr<UndergroundElementCurlyNode> parseUndergroundelementsCurlyChoice();
    std::unique_ptr<ElevatorVsebinaNode> parseElevatorVsebina();
    std::unique_ptr<OrebedVsebinaNode> parseOrebedVsebina();
    std::unique_ptr<UndergroundElementBracketNode> parseUndergroundelementsBracketChoice();
    std::unique_ptr<TunnelVsebinaNode> parseTunnelVsebina();
    std::unique_ptr<ShaftVsebinaNode> parseShaftVsebina();

    std::unique_ptr<SeaElementCurlyNode> parseSeaelementsElement();
    std::unique_ptr<SeaElementCurlyNode> parseSeaelementsCurlyChoice();
    std::unique_ptr<PlatformVsebinaNode> parsePlatformVsebina();
    std::unique_ptr<PumpVsebinaNode> parsePumpVsebina();
    std::unique_ptr<RiserVsebinaNode> parseRiserVsebina();

    std::unique_ptr<LinesNode> parseLines();
    std::unique_ptr<LinesContentNode> parseLinesContent();
    std::unique_ptr<LinesItemContentAfterParenNode> parseLinesItemContentAfterParen();
    std::unique_ptr<LinesBendOptNode> parseLinesBendOpt();

    std::unique_ptr<ArbitraryPolygonNode> parseArbitarraypolygon();
    std::unique_ptr<PolylineNode> parsePolyline();
    std::unique_ptr<LineNode> parseLine();
    std::unique_ptr<BoxNode> parseBox();
    std::unique_ptr<PointsNode> parsePoints();
    bool parsePointsPrime(PointsNode* ownerNode);
    std::unique_ptr<PointNode> parsePoint();
    std::unique_ptr<BendNode> parseBend();
    std::unique_ptr<CircNode> parseCirc();

    std::unique_ptr<ExprNode> parseExpr();
    std::unique_ptr<ExprNode> parseBitwise();
    std::unique_ptr<ExprNode> parseBitwisePrime(std::unique_ptr<ExprNode> lhs);
    std::unique_ptr<ExprNode> parseAdditive();
    std::unique_ptr<ExprNode> parseAdditivePrime(std::unique_ptr<ExprNode> lhs);
    std::unique_ptr<ExprNode> parseMultiplicative();
    std::unique_ptr<ExprNode> parseMultiplicativePrime(std::unique_ptr<ExprNode> lhs);
    std::unique_ptr<ExprNode> parseUnary();
    std::unique_ptr<ExprNode> parsePrimary();

    std::unique_ptr<CommandSetNode> parseCommandSet();
    bool parseCommandSetPrime(CommandSetNode* ownerNode);
    std::unique_ptr<CommandNode> parseCommand();
    std::unique_ptr<LoopNode> parseLoop();
    std::unique_ptr<AssignNode> parseAssign();
    std::unique_ptr<ExprNode> parseAssignDesno();
    std::unique_ptr<PrintNode> parsePrint();

    std::unique_ptr<FunctionNode> parseFunction();
    std::unique_ptr<AttributeNode> parseAtribute();
    bool parseAtributePrime(AttributeNode* ownerNode);
    std::unique_ptr<FunctionCallNode> parseFunctionCall();


public:
    Parser(Lexer lexer) : m_lexer(lexer) { 
        m_currentToken = m_lexer.nextToken();
        m_programNode = std::make_unique<ProgramNode>(0,0);
    }

    bool parseFile(const std::string& filePath);
    std::unique_ptr<ProgramNode> getProgramNode() { return std::move(m_programNode); }
};