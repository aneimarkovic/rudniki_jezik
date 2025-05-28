#pragma once

#include "Lexer.hpp"
#include "Ast.hpp" // Include the AST node definitions
#include <memory>   // For std::unique_ptr

/*
	Created by Žan Misja 26.05.2024
	Razred preveri ali je datoteka veljana glede na gramatiko BNF
*/

class Parser {
private:
    Lexer m_lexer;
    Token m_currentToken;
    std::unique_ptr<ProgramNode> m_programNode; // To store the complete AST

    // Helper to get current token's location for AstNode constructor
    Token currentTokenForAst() { return m_currentToken; }
    // Overload for when a token was just consumed, to get its original position
    Token previousTokenForAst(const Token& consumedToken) { return consumedToken; }


    bool match(int expectedType);
    bool tryToConsume(int expectedType, Token* consumedToken = nullptr); // Modified to capture consumed token
    void error(std::string errorMessage);

    // Parsing functions now return std::unique_ptr<AstNode_Type> or nullptr on failure
    // General Structure
    std::unique_ptr<StatementNode> parseBegin(); // Returns one top-level statement
    std::unique_ptr<BeginPrimeBodyNode> parseBeginPrime();
    std::unique_ptr<StatementNode> parseNonAssignCommand();
    std::unique_ptr<StartNode> parseStart();
    std::unique_ptr<MinetypeNode> parseMinetype();

    // Element Lists
    std::unique_ptr<LandElementsNode> parseLandelements();
    bool parseLandelementsPrime(LandElementsNode* ownerNode);
    std::unique_ptr<UndergroundElementsNode> parseUndergroundelements();
    bool parseUndergroundelementsPrime(UndergroundElementsNode* ownerNode);
    std::unique_ptr<SeaElementsNode> parseSeaelements();
    bool parseSeaelementsPrime(SeaElementsNode* ownerNode);

    // Specific Elements
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

    std::unique_ptr<SeaElementCurlyNode> parseSeaelementsElement(); // Direct to curly
    std::unique_ptr<SeaElementCurlyNode> parseSeaelementsCurlyChoice(); // Name is a bit confusing, it IS the element essentially
    std::unique_ptr<PlatformVsebinaNode> parsePlatformVsebina();
    std::unique_ptr<PumpVsebinaNode> parsePumpVsebina();
    std::unique_ptr<RiserVsebinaNode> parseRiserVsebina();

    // Geometric and Properties
    std::unique_ptr<LinesNode> parseLines(); // Returns a list of properties
    std::unique_ptr<LinesContentNode> parseLinesContent();
    std::unique_ptr<LinesItemContentAfterParenNode> parseLinesItemContentAfterParen();
    std::unique_ptr<LinesBendOptNode> parseLinesBendOpt(); // Can return nullptr for epsilon

    std::unique_ptr<ArbitraryPolygonNode> parseArbitarraypolygon();
    std::unique_ptr<PolylineNode> parsePolyline();
    std::unique_ptr<LineNode> parseLine();
    std::unique_ptr<BoxNode> parseBox();
    std::unique_ptr<PointsNode> parsePoints();
    bool parsePointsPrime(PointsNode* ownerNode); // Appends to owner
    std::unique_ptr<PointNode> parsePoint();
    std::unique_ptr<BendNode> parseBend();
    std::unique_ptr<CircNode> parseCirc();

    // Expressions (all return std::unique_ptr<ExprNode>)
    std::unique_ptr<ExprNode> parseExpr(); // Renamed from parseExpr to avoid conflict if ExprNode is the type
    std::unique_ptr<ExprNode> parseBitwise();
    std::unique_ptr<ExprNode> parseBitwisePrime(std::unique_ptr<ExprNode> lhs); // Helper for left-associativity
    std::unique_ptr<ExprNode> parseAdditive();
    std::unique_ptr<ExprNode> parseAdditivePrime(std::unique_ptr<ExprNode> lhs);
    std::unique_ptr<ExprNode> parseMultiplicative();
    std::unique_ptr<ExprNode> parseMultiplicativePrime(std::unique_ptr<ExprNode> lhs);
    std::unique_ptr<ExprNode> parseUnary();
    std::unique_ptr<ExprNode> parsePrimary(); // Returns specific primary types like IntLiteralNode etc. cast to ExprNode

    // Commands and Functions
    std::unique_ptr<CommandSetNode> parseCommandSet();
    bool parseCommandSetPrime(CommandSetNode* ownerNode); // Appends to owner
    std::unique_ptr<CommandNode> parseCommand(); // Returns LoopNode, AssignNode, or PrintNode
    std::unique_ptr<LoopNode> parseLoop();
    std::unique_ptr<AssignNode> parseAssign(); // Assign can be a statement or a command
    std::unique_ptr<ExprNode> parseAssignDesno(); // Returns the RHS expression of assignment
    std::unique_ptr<PrintNode> parsePrint();

    std::unique_ptr<FunctionNode> parseFunction();
    std::unique_ptr<AttributeNode> parseAtribute(); // Returns list of expressions
    bool parseAtributePrime(AttributeNode* ownerNode); // Appends to owner
    std::unique_ptr<FunctionCallNode> parseFunctionCall();


public:
    Parser(Lexer lexer) : m_lexer(lexer) { 
        m_currentToken = m_lexer.nextToken();
        m_programNode = std::make_unique<ProgramNode>(0,0); // Initialize program node
    }

    bool parseFile(const std::string& filePath);
    std::unique_ptr<ProgramNode> getProgramNode() { return std::move(m_programNode); } // To retrieve the AST
};