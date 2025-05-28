#include "Parser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

/*
    Parser.cpp
    Žan Misja 24.5.2025
    Darko Sever 28.5.2025 - AST
*/

bool Parser::tryToConsume(int expectedType, Token* consumedTokenInfo) {
    if (m_currentToken.getToken() == expectedType) {
        if (consumedTokenInfo) {
            *consumedTokenInfo = m_currentToken;
        }
        m_currentToken = m_lexer.nextToken();
        return true;
    }
    return false;
}

/*
    Funkcija parsa skozi file
*/
bool Parser::parseFile(const std::string& filePath) {
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        fprintf(stderr, "[ERROR] Parser: Could not open file %s\n", filePath.c_str());
        return false;
    }

    std::stringstream stringBuffer;
    stringBuffer << fileStream.rdbuf();
    fileStream.close();
    std::string sourceContent = stringBuffer.str();

    if (sourceContent.empty()) {
        m_programNode = std::make_unique<ProgramNode>(0,0);
        return true; 
    }
    
    m_programNode = std::make_unique<ProgramNode>(m_currentToken.getRow(), m_currentToken.getCol());

    try {
        while (!m_currentToken.isEof()) {
            auto statementNode = parseBegin();
            if (!statementNode) {
                return false;
            }
            m_programNode->statements.push_back(std::move(statementNode));
        }
        return true; 
    }
    catch (const std::runtime_error& e) {
        error("Runtime error during parsing: " + std::string(e.what()));
        return false;
    }
    catch (...) {
        error("Unknown exception during parsing.");
        return false;
    }
}

/*
    Funkcija, ki se uporablja za izpisvanje napak
*/
void Parser::error(std::string errorMessage) {
    fprintf(stderr, "[ERROR] %d:%d >> %s!\n", m_currentToken.getRow(), m_currentToken.getCol(), errorMessage.c_str());
}

/*
    Preveri ali je [m_currentToken] pravilne vrste, če je in vrne true
*/
bool Parser::match(int expectedType) {
    return m_currentToken.getToken() == expectedType;
}

/*
    Begin ::= Start
            | Function
            | Assign (CommandSet se obravnava kot zaporedje v parseFile)
            | Non_Assign_Command (CommandSet se obravnava kot zaporedje v parseFile)
            | Expr;
*/
std::unique_ptr<StatementNode> Parser::parseBegin() {
    Token startTok = currentTokenForAst();
    if (match(Lexer::tMine)) {
        return parseStart();
    } else if (match(Lexer::tFunction)) {
        return parseFunction();
    } else if (match(Lexer::tDouble)) {
        return parseAssign(); 
    } else if (match(Lexer::tFor) || match(Lexer::tConsole)) {
        return parseNonAssignCommand();
    }  else if (match(Lexer::tPlus) || match(Lexer::tOnlyPlus) ||
               match(Lexer::tMinus) ||
               match(Lexer::tInteger) || match(Lexer::tHex) ||
               match(Lexer::tVarLetter) || match(Lexer::tVarDigit) ||
               match(Lexer::tBracketOpen) || match(Lexer::tCall)) {
        auto expr = parseExpr();
        if (!expr) return nullptr;
        return std::make_unique<ExprStatementNode>(std::move(expr), startTok.getRow(), startTok.getCol());
    }

    error("Nepravilen začetek programa ali bloka (v parseBegin).");
    return nullptr;
}

/*
    Logika za: BeginPrime ::= CommandSet | Bitwise | epsilon;
*/
std::unique_ptr<BeginPrimeBodyNode> Parser::parseBeginPrime() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<BeginPrimeBodyNode>(startToken.getRow(), startToken.getCol());

    if (match(Lexer::tFor) || match(Lexer::tDouble) || match(Lexer::tConsole)) {
        auto commandSet = parseCommandSet();
        if (!commandSet) return nullptr;
        node->content = std::move(commandSet);
    } else if (match(Lexer::tPlus) || match(Lexer::tOnlyPlus) ||
               match(Lexer::tInteger) || match(Lexer::tMinus) ||
               match(Lexer::tHex) || match(Lexer::tVarLetter) ||
               match(Lexer::tVarDigit) || match(Lexer::tBracketOpen) ||
               match(Lexer::tCall)) {
        auto bitwiseExpr = parseBitwise();
        if (!bitwiseExpr) return nullptr;
        node->content = std::move(bitwiseExpr);
    } else {
        node->is_empty = true;
    }
    return node;
}

/*
    Logika za: Non_Assign_Command ::= Loop | Print;
*/
std::unique_ptr<StatementNode> Parser::parseNonAssignCommand() {
    if (match(Lexer::tFor)) {
        return parseLoop();
    } else if (match(Lexer::tConsole)) {
        return parsePrint();
    } else {
        error("Manjka Loop ali Print v NonAssignCommand.");
        return nullptr;
    }
}

/*
     Start ::= TOKEN_MINE STRING "{" Minetype "}";
*/
std::unique_ptr<StartNode> Parser::parseStart() {
    Token mineKeywordToken;
    if (!tryToConsume(Lexer::tMine, &mineKeywordToken)) {
        error("Manjka MINE.");
        return nullptr;
    }

    Token nameToken = currentTokenForAst();
    std::string mineNameValue;
    if (match(Lexer::tVarLetter) || match(Lexer::tVarDigit)) {
        mineNameValue = nameToken.getLexem();
        tryToConsume(m_currentToken.getToken());
    } else {
        error("Manjka ime rudnika.");
        return nullptr;
    }

    if (!tryToConsume(Lexer::tCurlyBracketOpen)) {
        error("Manjka '{' po imenu rudnika.");
        return nullptr;
    }
    
    auto minetypeNode = parseMinetype();
    if (!minetypeNode) return nullptr;

    if (!tryToConsume(Lexer::tCurlyBracketClosed)) {
        error("Manjka '}' na koncu definicije rudnika.");
        return nullptr;
    }
    return std::make_unique<StartNode>(mineNameValue, std::move(minetypeNode), mineKeywordToken.getRow(), mineKeywordToken.getCol());
}

/*
    Logika za
    Minetype ::= SURFACE "{" "[" Landelements "]" "}"
           | UNDERGROUND "{" "[" Undergroundelements "]" "}"
           | IN_PLACE "{" "[" Landelements "]" "}"
           | IN_SITU "{" "[" Landelements "]" "}"
           | DEEP_SEA "{" "[" Seaelements "]" "}";
*/
std::unique_ptr<MinetypeNode> Parser::parseMinetype() {
    Token typeToken = currentTokenForAst();
    MineType parsedMineType;

    if (match(Lexer::tSurface)) { parsedMineType = MineType::SURFACE; tryToConsume(Lexer::tSurface); }
    else if (match(Lexer::tUnderground)) { parsedMineType = MineType::UNDERGROUND; tryToConsume(Lexer::tUnderground); }
    else if (match(Lexer::tInplace)) { parsedMineType = MineType::IN_PLACE; tryToConsume(Lexer::tInplace); }
    else if (match(Lexer::tInsitu)) { parsedMineType = MineType::IN_SITU; tryToConsume(Lexer::tInsitu); }
    else if (match(Lexer::tDeepsea)) { parsedMineType = MineType::DEEP_SEA; tryToConsume(Lexer::tDeepsea); }
    else {
        error("Manjka veljaven tip rudnika (SURFACE, UNDERGROUND, IN_PLACE, IN_SITU, DEEP_SEA).");
        return nullptr;
    }
    
    auto node = std::make_unique<MinetypeNode>(parsedMineType, typeToken.getRow(), typeToken.getCol());

    if (!tryToConsume(Lexer::tCurlyBracketOpen)) {
        error("Manjka '{' po tipu rudnika.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tSquareBracketOpen)) {
        error("Manjka '[' po '{' v definiciji Minetype.");
        return nullptr;
    }

    if (parsedMineType == MineType::SURFACE || parsedMineType == MineType::IN_PLACE || parsedMineType == MineType::IN_SITU) {
        auto landElements = parseLandelements();
        if (!landElements) return nullptr;
        node->elements_content = std::move(landElements);
    } else if (parsedMineType == MineType::UNDERGROUND) {
        auto undergroundElements = parseUndergroundelements();
        if (!undergroundElements) return nullptr;
        node->elements_content = std::move(undergroundElements);
    } else if (parsedMineType == MineType::DEEP_SEA) {
        auto seaElements = parseSeaelements();
        if (!seaElements) return nullptr;
        node->elements_content = std::move(seaElements);
    }

    if (!tryToConsume(Lexer::tSquareBracketClosed)) {
        error("Manjka ']' v definiciji Minetype.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tCurlyBracketClosed)) {
        error("Manjka '}' na koncu definicije Minetype.");
        return nullptr;
    }
    return node;
}

std::unique_ptr<LandElementsNode> Parser::parseLandelements() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<LandElementsNode>(startToken.getRow(), startToken.getCol());
    auto firstElement = parseLandelementsElement();
    if (!firstElement) {
         error("Pričakovan vsaj en Landelement.");
        return nullptr;
    }
    node->elements.push_back(std::move(firstElement));
    if (!parseLandelementsPrime(node.get())) return nullptr;
    return node;
}

bool Parser::parseLandelementsPrime(LandElementsNode* ownerNode) {
    if (match(Lexer::tComma)) {
        tryToConsume(Lexer::tComma);
        auto nextElement = parseLandelementsElement();
        if (!nextElement) {
            return false;
        }
        ownerNode->elements.push_back(std::move(nextElement));
        return parseLandelementsPrime(ownerNode);
    }
    return true;
}

std::unique_ptr<LandElementNode> Parser::parseLandelementsElement() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<LandElementNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tCurlyBracketOpen)) {
        tryToConsume(Lexer::tCurlyBracketOpen);
        auto curlyChoice = parseLandelementsCurlyChoice();
        if (!curlyChoice) return nullptr;
        if (!tryToConsume(Lexer::tCurlyBracketClosed)) {
            error("Manjka '}' pri Landelements_Element (curly).");
            return nullptr;
        }
        node->element_choice = std::move(curlyChoice);
    } else if (match(Lexer::tSquareBracketOpen)) {
        tryToConsume(Lexer::tSquareBracketOpen);
        auto bracketChoice = parseLandelementsBracketChoice();
        if (!bracketChoice) return nullptr;
        if (!tryToConsume(Lexer::tSquareBracketClosed)) {
            error("Manjka ']' pri Landelements_Element (bracket).");
            return nullptr;
        }
        node->element_choice = std::move(bracketChoice);
    } else {
        error("Pričakovan '{' ali '[' na začetku Landelements_Element.");
        return nullptr;
    }
    return node;
}

std::unique_ptr<LandElementCurlyNode> Parser::parseLandelementsCurlyChoice() {
    Token typeToken = currentTokenForAst();
    if (match(Lexer::tPath)) {
        tryToConsume(Lexer::tPath);
        auto node = std::make_unique<LandElementCurlyNode>(LandElementCurlyType::PATH, typeToken.getRow(), typeToken.getCol());
        node->path_vsebina = parsePathVsebina();
        if (!node->path_vsebina) return nullptr;
        return node;
    } else if (match(Lexer::tRiver)) {
        tryToConsume(Lexer::tRiver);
        auto node = std::make_unique<LandElementCurlyNode>(LandElementCurlyType::RIVER, typeToken.getRow(), typeToken.getCol());
        node->river_vsebina = parseRiverVsebina();
        if (!node->river_vsebina) return nullptr;
        return node;
    } else {
        error("Nepričakovan žeton v Landelements_Curly_Choice. Pričakovan PATH ali RIVER.");
        return nullptr;
    }
}

std::unique_ptr<PathVsebinaNode> Parser::parsePathVsebina() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<PathVsebinaNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tLine)) {
        auto lineShape = parseLine();
        if (!lineShape) return nullptr;
        node->shape = std::move(lineShape);
    } else if (match(Lexer::tPolyline)) {
        auto polylineShape = parsePolyline();
        if (!polylineShape) return nullptr;
        node->shape = std::move(polylineShape);
    } else if (match(Lexer::tBend)) {
        auto bendShape = parseBend();
        if (!bendShape) return nullptr;
        node->shape = std::move(bendShape);
    } else {
        error("Path_Vsebina pričakuje Line, Polyline ali Bend.");
        return nullptr;
    }
    node->lines_properties = parseLines();
    if(!node->lines_properties) return nullptr;
    return node;
}

std::unique_ptr<RiverVsebinaNode> Parser::parseRiverVsebina() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<RiverVsebinaNode>(startToken.getRow(), startToken.getCol());
     if (match(Lexer::tLine)) {
        auto lineShape = parseLine();
        if (!lineShape) return nullptr;
        node->shape = std::move(lineShape);
    } else if (match(Lexer::tPolyline)) {
        auto polylineShape = parsePolyline();
        if (!polylineShape) return nullptr;
        node->shape = std::move(polylineShape);
    } else if (match(Lexer::tBend)) {
        auto bendShape = parseBend();
        if (!bendShape) return nullptr;
        node->shape = std::move(bendShape);
    } else {
        error("River_Vsebina pričakuje Line, Polyline ali Bend.");
        return nullptr;
    }
    node->lines_properties = parseLines();
    if(!node->lines_properties) return nullptr;
    return node;
}

std::unique_ptr<LandElementBracketNode> Parser::parseLandelementsBracketChoice() {
    Token typeToken = currentTokenForAst();
    LandElementBracketType bracketType;
    std::unique_ptr<ElementVsebinaNode> vsebinaNode;

    if (match(Lexer::tStrip)) {
        tryToConsume(Lexer::tStrip); bracketType = LandElementBracketType::STRIP;
        vsebinaNode = parseStripVsebina();
    } else if (match(Lexer::tBench)) {
        tryToConsume(Lexer::tBench); bracketType = LandElementBracketType::BENCH;
        vsebinaNode = parseBenchVsebina();
    } else if (match(Lexer::tSpoilpit)) {
        tryToConsume(Lexer::tSpoilpit); bracketType = LandElementBracketType::SPOILPIT;
        vsebinaNode = parseSpoilpitVsebina();
    } else if (match(Lexer::tPlant)) {
        tryToConsume(Lexer::tPlant); bracketType = LandElementBracketType::PLANT;
        vsebinaNode = parsePlantVsebina();
    } else if (match(Lexer::tWell)) {
        tryToConsume(Lexer::tWell); bracketType = LandElementBracketType::WELL;
        vsebinaNode = parseWellVsebina();
    } else {
        error("Nepričakovan žeton v Landelements_Bracket_Choice.");
        return nullptr;
    }
    if (!vsebinaNode) return nullptr;
    return std::make_unique<LandElementBracketNode>(bracketType, std::move(vsebinaNode), typeToken.getRow(), typeToken.getCol());
}

std::unique_ptr<StripVsebinaNode> Parser::parseStripVsebina() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<StripVsebinaNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tPolygon)) {
        auto poly = parseArbitarraypolygon();
        if (!poly) return nullptr;
        node->shape = std::move(poly);
    } else if (match(Lexer::tBox)) {
        auto box = parseBox();
        if (!box) return nullptr;
        node->shape = std::move(box);
    } else {
        error("Strip_Vsebina pričakuje Polygon ali Box.");
        return nullptr;
    }
    return node;
}

std::unique_ptr<BenchVsebinaNode> Parser::parseBenchVsebina() {
    Token startToken = currentTokenForAst();
    if (match(Lexer::tPolygon)) {
        auto poly = parseArbitarraypolygon();
        if (!poly) return nullptr;
        return std::make_unique<BenchVsebinaNode>(std::move(poly), startToken.getRow(), startToken.getCol());
    } else {
        error("Bench_Vsebina pričakuje Polygon.");
        return nullptr;
    }
}

std::unique_ptr<SpoilpitVsebinaNode> Parser::parseSpoilpitVsebina() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<SpoilpitVsebinaNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tPolygon)) {
        auto poly = parseArbitarraypolygon();
        if(!poly) return nullptr;
        node->shape = std::move(poly);
    } else if (match(Lexer::tBox)) {
        auto box = parseBox();
        if(!box) return nullptr;
        node->shape = std::move(box);
    } else if (match(Lexer::tCircle)) {
        auto circ = parseCirc();
        if(!circ) return nullptr;
        node->shape = std::move(circ);
    } else {
        error("Spoilpit_Vsebina pričakuje Polygon, Box ali Circle.");
        return nullptr;
    }
    return node;
}

std::unique_ptr<PlantVsebinaNode> Parser::parsePlantVsebina() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<PlantVsebinaNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tPolygon)) {
        auto poly = parseArbitarraypolygon();
        if(!poly) return nullptr;
        node->shape = std::move(poly);
    } else if (match(Lexer::tBox)) {
        auto box = parseBox();
        if(!box) return nullptr;
        node->shape = std::move(box);
    } else if (match(Lexer::tCircle)) {
        auto circ = parseCirc();
        if(!circ) return nullptr;
        node->shape = std::move(circ);
    } else {
        error("Plant_Vsebina pričakuje Polygon, Box ali Circle.");
        return nullptr;
    }
    return node;
}

std::unique_ptr<WellVsebinaNode> Parser::parseWellVsebina() {
    Token typeToken = currentTokenForAst();
    WellType wellTypeEnum;
    if (match(Lexer::tMonitoring)) { wellTypeEnum = WellType::MONITORING; tryToConsume(Lexer::tMonitoring); }
    else if (match(Lexer::tInjection)) { wellTypeEnum = WellType::INJECTION; tryToConsume(Lexer::tInjection); }
    else if (match(Lexer::tRecovery)) { wellTypeEnum = WellType::RECOVERY; tryToConsume(Lexer::tRecovery); }
    else {
        error("Well_Vsebina pričakuje MONITORING, INJECTION, ali RECOVERY.");
        return nullptr;
    }
    auto pointNode = parsePoint();
    if (!pointNode) return nullptr;
    return std::make_unique<WellVsebinaNode>(wellTypeEnum, std::move(pointNode), typeToken.getRow(), typeToken.getCol());
}

std::unique_ptr<UndergroundElementsNode> Parser::parseUndergroundelements() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<UndergroundElementsNode>(startToken.getRow(), startToken.getCol());
    auto firstElement = parseUndergroundelementsElement();
    if (!firstElement) return nullptr;
    node->elements.push_back(std::move(firstElement));
    if (!parseUndergroundelementsPrime(node.get())) return nullptr;
    return node;
}

bool Parser::parseUndergroundelementsPrime(UndergroundElementsNode* ownerNode) {
    if (match(Lexer::tComma)) {
        tryToConsume(Lexer::tComma);
        auto nextElement = parseUndergroundelementsElement();
        if (!nextElement) return false;
        ownerNode->elements.push_back(std::move(nextElement));
        return parseUndergroundelementsPrime(ownerNode);
    }
    return true;
}

std::unique_ptr<UndergroundElementNode> Parser::parseUndergroundelementsElement() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<UndergroundElementNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tCurlyBracketOpen)) {
        tryToConsume(Lexer::tCurlyBracketOpen);
        auto curlyChoice = parseUndergroundelementsCurlyChoice();
        if (!curlyChoice) return nullptr;
        if (!tryToConsume(Lexer::tCurlyBracketClosed)) {
            error("Manjka '}' pri Undergroundelements_Element (curly).");
            return nullptr;
        }
        node->element_choice = std::move(curlyChoice);
    } else if (match(Lexer::tSquareBracketOpen)) {
        tryToConsume(Lexer::tSquareBracketOpen);
        auto bracketChoice = parseUndergroundelementsBracketChoice();
        if (!bracketChoice) return nullptr;
        if (!tryToConsume(Lexer::tSquareBracketClosed)) {
            error("Manjka ']' pri Undergroundelements_Element (bracket).");
            return nullptr;
        }
        node->element_choice = std::move(bracketChoice);
    } else {
        error("Pričakovan '{' ali '[' na začetku Undergroundelements_Element.");
        return nullptr;
    }
    return node;
}

std::unique_ptr<UndergroundElementCurlyNode> Parser::parseUndergroundelementsCurlyChoice() {
    Token typeToken = currentTokenForAst();
    UndergroundElementCurlyType curlyType;
    std::unique_ptr<ElementVsebinaNode> vsebinaNode;

    if (match(Lexer::tElevator)) {
        tryToConsume(Lexer::tElevator); curlyType = UndergroundElementCurlyType::ELEVATOR;
        vsebinaNode = parseElevatorVsebina();
    } else if (match(Lexer::tOreBed)) {
        tryToConsume(Lexer::tOreBed); curlyType = UndergroundElementCurlyType::OREBED;
        vsebinaNode = parseOrebedVsebina();
    } else {
        error("Nepričakovan žeton v Undergroundelements_Curly_Choice. Pričakovan ELEVATOR ali OREBED.");
        return nullptr;
    }
    if (!vsebinaNode) return nullptr;
    return std::make_unique<UndergroundElementCurlyNode>(curlyType, std::move(vsebinaNode), typeToken.getRow(), typeToken.getCol());
}

std::unique_ptr<ElevatorVsebinaNode> Parser::parseElevatorVsebina() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<ElevatorVsebinaNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tHuman)) {
        tryToConsume(Lexer::tHuman);
        node->content = ElevatorType::HUMAN;
    } else if (match(Lexer::tOre)) {
        tryToConsume(Lexer::tOre);
        node->content = ElevatorType::ORE;
    } else if (match(Lexer::tPoint)) {
        auto pointNode = parsePoint();
        if (!pointNode) return nullptr;
        node->content = std::move(pointNode);
    } else {
        error("Elevator_Vsebina pričakuje HUMAN, ORE, ali Point.");
        return nullptr;
    }
    return node;
}

std::unique_ptr<OrebedVsebinaNode> Parser::parseOrebedVsebina() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<OrebedVsebinaNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tPolygon)) {
        auto poly = parseArbitarraypolygon();
        if(!poly) return nullptr;
        node->shape = std::move(poly);
    } else if (match(Lexer::tBox)) {
        auto box = parseBox();
        if(!box) return nullptr;
        node->shape = std::move(box);
    } else if (match(Lexer::tCircle)) {
        auto circ = parseCirc();
        if(!circ) return nullptr;
        node->shape = std::move(circ);
    } else {
        error("Orebed_Vsebina pričakuje Polygon, Box ali Circle.");
        return nullptr;
    }
    return node;
}

std::unique_ptr<UndergroundElementBracketNode> Parser::parseUndergroundelementsBracketChoice() {
    Token typeToken = currentTokenForAst();
    UndergroundElementBracketType bracketType;
    std::unique_ptr<ElementVsebinaNode> vsebinaNode;

    if (match(Lexer::tTunnel)) {
        tryToConsume(Lexer::tTunnel); bracketType = UndergroundElementBracketType::TUNNEL;
        vsebinaNode = parseTunnelVsebina();
    } else if (match(Lexer::tShaft)) {
        tryToConsume(Lexer::tShaft); bracketType = UndergroundElementBracketType::SHAFT;
        vsebinaNode = parseShaftVsebina();
    } else {
        error("Nepričakovan žeton v Undergroundelements_Bracket_Choice. Pričakovan TUNNEL ali SHAFT.");
        return nullptr;
    }
    if (!vsebinaNode) return nullptr;
    return std::make_unique<UndergroundElementBracketNode>(bracketType, std::move(vsebinaNode), typeToken.getRow(), typeToken.getCol());
}

std::unique_ptr<TunnelVsebinaNode> Parser::parseTunnelVsebina() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<TunnelVsebinaNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tPolyline)) {
        auto polylineShape = parsePolyline();
        if (!polylineShape) return nullptr;
        node->shape = std::move(polylineShape);
    } else if (match(Lexer::tLine)) {
        auto lineShape = parseLine();
        if (!lineShape) return nullptr;
        node->shape = std::move(lineShape);
    } else if (match(Lexer::tBend)) {
        auto bendShape = parseBend();
        if (!bendShape) return nullptr;
        node->shape = std::move(bendShape);
    } else {
        error("Tunnel_Vsebina pričakuje Polyline, Line ali Bend.");
        return nullptr;
    }
    node->lines_properties = parseLines();
    if(!node->lines_properties) return nullptr;
    return node;
}

std::unique_ptr<ShaftVsebinaNode> Parser::parseShaftVsebina() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<ShaftVsebinaNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tPolyline)) {
        auto polylineShape = parsePolyline();
        if (!polylineShape) return nullptr;
        node->shape = std::move(polylineShape);
    } else if (match(Lexer::tLine)) {
        auto lineShape = parseLine();
        if (!lineShape) return nullptr;
        node->shape = std::move(lineShape);
    } else if (match(Lexer::tBend)) {
        auto bendShape = parseBend();
        if (!bendShape) return nullptr;
        node->shape = std::move(bendShape);
    } else {
        error("Shaft_Vsebina pričakuje Polyline, Line ali Bend.");
        return nullptr;
    }
    node->lines_properties = parseLines();
    if(!node->lines_properties) return nullptr;
    return node;
}

// --- Seaelements ---
// Seaelements ::= Seaelements_Element Seaelements';
// Seaelements_Element ::= "{" Seaelements_Curly_Choice "}";
// Torej parseSeaelementsElement() dejansko razčleni SeaElementCurlyNode
std::unique_ptr<SeaElementsNode> Parser::parseSeaelements() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<SeaElementsNode>(startToken.getRow(), startToken.getCol());
    auto firstElement = parseSeaelementsElement();
    if (!firstElement) return nullptr;
    node->elements.push_back(std::move(firstElement));
    if (!parseSeaelementsPrime(node.get())) return nullptr;
    return node;
}

bool Parser::parseSeaelementsPrime(SeaElementsNode* ownerNode) {
    if (match(Lexer::tComma)) {
        tryToConsume(Lexer::tComma);
        auto nextElement = parseSeaelementsElement();
        if (!nextElement) return false;
        ownerNode->elements.push_back(std::move(nextElement));
        return parseSeaelementsPrime(ownerNode);
    }
    return true;
}

std::unique_ptr<SeaElementCurlyNode> Parser::parseSeaelementsElement() {
    Token openCurlyToken;
    if (!tryToConsume(Lexer::tCurlyBracketOpen, &openCurlyToken)) {
        error("Manjka '{' na začetku Seaelements_Element.");
        return nullptr;
    }
    auto curlyChoice = parseSeaelementsCurlyChoice();
    if (!curlyChoice) return nullptr;
    
    if (!tryToConsume(Lexer::tCurlyBracketClosed)) {
        error("Manjka '}' na koncu Seaelements_Element.");
        return nullptr;
    }
    return curlyChoice;
}

std::unique_ptr<SeaElementCurlyNode> Parser::parseSeaelementsCurlyChoice() {
    Token typeToken = currentTokenForAst();
    SeaElementCurlyType curlyType;
    std::unique_ptr<ElementVsebinaNode> vsebinaNode;

    if (match(Lexer::tPlatform)) {
        tryToConsume(Lexer::tPlatform); curlyType = SeaElementCurlyType::PLATFORM;
        vsebinaNode = parsePlatformVsebina();
    } else if (match(Lexer::tPump)) {
        tryToConsume(Lexer::tPump); curlyType = SeaElementCurlyType::PUMP;
        vsebinaNode = parsePumpVsebina();
    } else if (match(Lexer::tRiser)) {
        tryToConsume(Lexer::tRiser); curlyType = SeaElementCurlyType::RISER;
        vsebinaNode = parseRiserVsebina();
    } else {
        error("Nepričakovan žeton v Seaelements_Curly_Choice. Pričakovan PLATFORM, PUMP, ali RISER.");
        return nullptr;
    }
    if (!vsebinaNode) return nullptr;
    return std::make_unique<SeaElementCurlyNode>(curlyType, std::move(vsebinaNode), typeToken.getRow(), typeToken.getCol());
}

std::unique_ptr<PlatformVsebinaNode> Parser::parsePlatformVsebina() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<PlatformVsebinaNode>(startToken.getRow(), startToken.getCol());
    if (match(Lexer::tPolygon)) {
        auto poly = parseArbitarraypolygon();
        if(!poly) return nullptr;
        node->shape = std::move(poly);
    } else if (match(Lexer::tBox)) {
        auto box = parseBox();
        if(!box) return nullptr;
        node->shape = std::move(box);
    } else if (match(Lexer::tCircle)) {
        auto circ = parseCirc();
        if(!circ) return nullptr;
        node->shape = std::move(circ);
    } else {
        error("Platform_Vsebina pričakuje Polygon, Box ali Circle.");
        return nullptr;
    }
    return node;
}

std::unique_ptr<PumpVsebinaNode> Parser::parsePumpVsebina() {
    Token startToken = currentTokenForAst();
    auto pointNode = parsePoint();
    if (!pointNode) return nullptr;
    return std::make_unique<PumpVsebinaNode>(std::move(pointNode), startToken.getRow(), startToken.getCol());
}

std::unique_ptr<RiserVsebinaNode> Parser::parseRiserVsebina() {
    Token startToken = currentTokenForAst();
    auto pointNode = parsePoint();
    if (!pointNode) return nullptr;
    return std::make_unique<RiserVsebinaNode>(std::move(pointNode), startToken.getRow(), startToken.getCol());
}

// --- Geometric and Properties ---
// Lines ::= "," "{" STRING Lines_Content "}" Lines | ε;
std::unique_ptr<LinesNode> Parser::parseLines() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<LinesNode>(startToken.getRow(), startToken.getCol());

    while (match(Lexer::tComma)) {
        Token commaToken = currentTokenForAst();
        tryToConsume(Lexer::tComma);

        Token openCurlyToken;
        if (!tryToConsume(Lexer::tCurlyBracketOpen, &openCurlyToken)) {
            error("Manjka '{' v Lines produkciji.");
            return nullptr; // Napaka
        }

        Token stringToken = currentTokenForAst();
        std::string propName;
        if (match(Lexer::tVarLetter) || match(Lexer::tVarDigit)) {
            propName = stringToken.getLexem();
            tryToConsume(m_currentToken.getToken());
        } else {
            error("Manjka ime lastnosti (STRING) v Lines produkciji.");
            return nullptr;
        }

        auto linesContent = parseLinesContent();
        if (!linesContent) return nullptr;

        if (!tryToConsume(Lexer::tCurlyBracketClosed)) {
            error("Manjka '}' v Lines produkciji.");
            return nullptr;
        }
        node->properties.push_back(std::make_unique<LinePropertyNode>(
            std::string(propName),
            std::move(linesContent),
            openCurlyToken.getRow(),
            openCurlyToken.getCol()
        ));
    }
    return node;
}

// Lines_Content ::= "(" Lines_Item_Content_After_Paren ")";
std::unique_ptr<LinesContentNode> Parser::parseLinesContent() {
    Token openParenToken;
    if (!tryToConsume(Lexer::tBracketOpen, &openParenToken)) {
        error("Manjka '(' v Lines_Content.");
        return nullptr;
    }
    auto itemContent = parseLinesItemContentAfterParen();
    if (!itemContent) return nullptr;
    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' v Lines_Content.");
        return nullptr;
    }
    return std::make_unique<LinesContentNode>(std::move(itemContent), openParenToken.getRow(), openParenToken.getCol());
}

// Lines_Item_Content_After_Paren ::= "[" Points "]" | Point "," Point Lines_Bend_Opt;
std::unique_ptr<LinesItemContentAfterParenNode> Parser::parseLinesItemContentAfterParen() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<LinesItemContentAfterParenNode>(startToken.getRow(), startToken.getCol());

    if (match(Lexer::tSquareBracketOpen)) {
        tryToConsume(Lexer::tSquareBracketOpen);
        auto pointsNode = parsePoints();
        if (!pointsNode) return nullptr;
        if (!tryToConsume(Lexer::tSquareBracketClosed)) {
            error("Manjka ']' v Lines_Item_Content_After_Paren (Points).");
            return nullptr;
        }
        node->content = std::move(pointsNode);
    } else if (match(Lexer::tPoint)) {
        auto p1 = parsePoint();
        if (!p1) return nullptr;
        if (!tryToConsume(Lexer::tComma)) {
            error("Manjka ',' po prvem Point v Lines_Item_Content_After_Paren.");
            return nullptr;
        }
        auto p2 = parsePoint();
        if (!p2) return nullptr;
        auto bendOpt = parseLinesBendOpt();
        node->content = std::make_tuple(std::move(p1), std::move(p2), std::move(bendOpt));
    } else {
        error("Nepričakovana vsebina v Lines_Item_Content_After_Paren. Pričakovan '[' ali Point.");
        return nullptr;
    }
    return node;
}

// Lines_Bend_Opt ::= "," Bitwise | ε;
std::unique_ptr<LinesBendOptNode> Parser::parseLinesBendOpt() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<LinesBendOptNode>(nullptr, startToken.getRow(), startToken.getCol());
    if (match(Lexer::tComma)) {
        tryToConsume(Lexer::tComma);
        auto bitwiseExpr = parseBitwise();
        if (!bitwiseExpr) {
            error("Napaka pri razčlenjevanju Bitwise v Lines_Bend_Opt po vejici.");
            return nullptr;
        }
        node->value = std::move(bitwiseExpr);
    }
    return node;
}


std::unique_ptr<ArbitraryPolygonNode> Parser::parseArbitarraypolygon() {
    Token polyToken;
    if (!tryToConsume(Lexer::tPolygon, &polyToken)) {
        error("Manjka POLYGON.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketOpen)) {
        error("Manjka '(' za POLYGON.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tSquareBracketOpen)) {
        error("Manjka '[' za POLYGON.");
        return nullptr;
    }
    auto points = parsePoints();
    if (!points) return nullptr;
    if (!tryToConsume(Lexer::tSquareBracketClosed)) {
        error("Manjka ']' za POLYGON.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' za POLYGON.");
        return nullptr;
    }
    return std::make_unique<ArbitraryPolygonNode>(std::move(points), polyToken.getRow(), polyToken.getCol());
}

std::unique_ptr<PolylineNode> Parser::parsePolyline() {
    Token polylineToken;
    if (!tryToConsume(Lexer::tPolyline, &polylineToken)) {
        error("Manjka POLYLINE.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketOpen)) {
        error("Manjka '(' za POLYLINE.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tSquareBracketOpen)) {
        error("Manjka '[' za POLYLINE.");
        return nullptr;
    }
    auto points = parsePoints();
    if (!points) return nullptr;
    if (!tryToConsume(Lexer::tSquareBracketClosed)) {
        error("Manjka ']' za POLYLINE.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' za POLYLINE.");
        return nullptr;
    }
    return std::make_unique<PolylineNode>(std::move(points), polylineToken.getRow(), polylineToken.getCol());
}

std::unique_ptr<LineNode> Parser::parseLine() {
    Token lineToken;
    if (!tryToConsume(Lexer::tLine, &lineToken)) {
        error("Manjka LINE.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketOpen)) {
        error("Manjka '(' za LINE.");
        return nullptr;
    }
    auto p1 = parsePoint();
    if (!p1) return nullptr;
    if (!tryToConsume(Lexer::tComma)) {
        error("Manjka ',' med točkami v LINE.");
        return nullptr;
    }
    auto p2 = parsePoint();
    if (!p2) return nullptr;
    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' za LINE.");
        return nullptr;
    }
    return std::make_unique<LineNode>(std::move(p1), std::move(p2), lineToken.getRow(), lineToken.getCol());
}

std::unique_ptr<BoxNode> Parser::parseBox() {
    Token boxToken;
    if (!tryToConsume(Lexer::tBox, &boxToken)) {
        error("Manjka BOX.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketOpen)) {
        error("Manjka '(' za BOX.");
        return nullptr;
    }
    auto p1 = parsePoint();
    if (!p1) return nullptr;
    if (!tryToConsume(Lexer::tComma)) {
        error("Manjka ',' med točkami v BOX.");
        return nullptr;
    }
    auto p2 = parsePoint();
    if (!p2) return nullptr;
    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' za BOX.");
        return nullptr;
    }
    return std::make_unique<BoxNode>(std::move(p1), std::move(p2), boxToken.getRow(), boxToken.getCol());
}

std::unique_ptr<PointsNode> Parser::parsePoints() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<PointsNode>(startToken.getRow(), startToken.getCol());
    auto firstPoint = parsePoint();
    if (!firstPoint) return nullptr;
    node->points_list.push_back(std::move(firstPoint));
    if (!parsePointsPrime(node.get())) return nullptr;
    return node;
}

bool Parser::parsePointsPrime(PointsNode* ownerNode) {
    if (match(Lexer::tComma)) {
        tryToConsume(Lexer::tComma);
        auto nextPoint = parsePoint();
        if (!nextPoint) return false;
        ownerNode->points_list.push_back(std::move(nextPoint));
        return parsePointsPrime(ownerNode);
    }
    return true;
}

std::unique_ptr<PointNode> Parser::parsePoint() {
    Token pointToken;
    if (!tryToConsume(Lexer::tPoint, &pointToken)) {
        error("Manjka POINT.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketOpen)) {
        error("Manjka '(' za POINT.");
        return nullptr;
    }
    auto x_expr = parseBitwise();
    if (!x_expr) return nullptr;
    if (!tryToConsume(Lexer::tComma)) {
        error("Manjka ',' med koordinatami POINT.");
        return nullptr;
    }
    auto y_expr = parseBitwise();
    if (!y_expr) return nullptr;
    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' za POINT.");
        return nullptr;
    }
    return std::make_unique<PointNode>(std::move(x_expr), std::move(y_expr), pointToken.getRow(), pointToken.getCol());
}

std::unique_ptr<BendNode> Parser::parseBend() {
    Token bendToken;
    if (!tryToConsume(Lexer::tBend, &bendToken)) {
        error("Manjka BEND.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketOpen)) {
        error("Manjka '(' za BEND.");
        return nullptr;
    }
    auto p1 = parsePoint();
    if (!p1) return nullptr;
    if (!tryToConsume(Lexer::tComma)) {
        error("Manjka ',' po prvem Point v BEND.");
        return nullptr;
    }
    auto p2 = parsePoint();
    if (!p2) return nullptr;
    if (!tryToConsume(Lexer::tComma)) {
        error("Manjka ',' po drugem Point v BEND.");
        return nullptr;
    }
    auto bitwise_expr = parseBitwise();
    if (!bitwise_expr) return nullptr;
    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' za BEND.");
        return nullptr;
    }
    return std::make_unique<BendNode>(std::move(p1), std::move(p2), std::move(bitwise_expr), bendToken.getRow(), bendToken.getCol());
}

std::unique_ptr<CircNode> Parser::parseCirc() {
    Token circToken;
    if (!tryToConsume(Lexer::tCircle, &circToken)) {
        error("Manjka CIRCLE.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketOpen)) {
        error("Manjka '(' za CIRCLE.");
        return nullptr;
    }
    auto center_point = parsePoint();
    if (!center_point) return nullptr;
    if (!tryToConsume(Lexer::tComma)) {
        error("Manjka ',' po Point v CIRCLE.");
        return nullptr;
    }
    auto radius_expr = parseBitwise();
    if (!radius_expr) return nullptr;
    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' za CIRCLE.");
        return nullptr;
    }
    return std::make_unique<CircNode>(std::move(center_point), std::move(radius_expr), circToken.getRow(), circToken.getCol());
}

// --- Expressions ---
std::unique_ptr<ExprNode> Parser::parseExpr() {
    return parseBitwise();
}

std::unique_ptr<ExprNode> Parser::parseBitwise() {
    auto lhs = parseAdditive();
    if (!lhs) return nullptr;
    return parseBitwisePrime(std::move(lhs));
}

std::unique_ptr<ExprNode> Parser::parseBitwisePrime(std::unique_ptr<ExprNode> lhs) {
    Token opToken = currentTokenForAst();
    if (match(Lexer::tAND) || match(Lexer::tOR)) {
        BinaryOperatorType op_type;
        if (match(Lexer::tAND)) { op_type = BinaryOperatorType::BW_AND; tryToConsume(Lexer::tAND); }
        else { op_type = BinaryOperatorType::BW_OR; tryToConsume(Lexer::tOR); }
        
        auto rhs = parseAdditive();
        if (!rhs) {
            error("Pričakovan izraz za desno stran '" + opToken.getLexem() + "' v parseBitwisePrime.");
            return nullptr;
        }
        auto new_lhs = std::make_unique<BinaryOpNode>(std::move(lhs), op_type, std::move(rhs), opToken.getRow(), opToken.getCol());
        return parseBitwisePrime(std::move(new_lhs));
    }
    return lhs; 
}

std::unique_ptr<ExprNode> Parser::parseAdditive() {
    auto lhs = parseMultiplicative();
    if (!lhs) return nullptr;
    return parseAdditivePrime(std::move(lhs));
}

std::unique_ptr<ExprNode> Parser::parseAdditivePrime(std::unique_ptr<ExprNode> lhs) {
    Token opToken = currentTokenForAst();
    if (match(Lexer::tPlus) || match(Lexer::tOnlyPlus) || match(Lexer::tMinus)) {
        BinaryOperatorType op_type;
        if (match(Lexer::tPlus) || match(Lexer::tOnlyPlus)) { 
            op_type = BinaryOperatorType::PLUS; 
            tryToConsume(m_currentToken.getToken());
        } else { 
            op_type = BinaryOperatorType::MINUS; 
            tryToConsume(Lexer::tMinus); 
        }
        
        auto rhs = parseMultiplicative();
        if (!rhs) {
            error("Pričakovan izraz za desno stran '" + opToken.getLexem() + "' v parseAdditivePrime.");
            return nullptr;
        }
        auto new_lhs = std::make_unique<BinaryOpNode>(std::move(lhs), op_type, std::move(rhs), opToken.getRow(), opToken.getCol());
        return parseAdditivePrime(std::move(new_lhs));
    }
    return lhs; 
}

std::unique_ptr<ExprNode> Parser::parseMultiplicative() {
    auto lhs = parseUnary();
    if (!lhs) return nullptr;
    return parseMultiplicativePrime(std::move(lhs));
}

std::unique_ptr<ExprNode> Parser::parseMultiplicativePrime(std::unique_ptr<ExprNode> lhs) {
    Token opToken = currentTokenForAst();
    if (match(Lexer::tTimes) || match(Lexer::tOnlyTimes) || match(Lexer::tSlash)) {
        BinaryOperatorType op_type;
        if (match(Lexer::tTimes) || match(Lexer::tOnlyTimes)) { 
            op_type = BinaryOperatorType::TIMES; 
            tryToConsume(m_currentToken.getToken());
        } else { 
            op_type = BinaryOperatorType::DIVIDE; 
            tryToConsume(Lexer::tSlash); 
        }
        
        auto rhs = parseUnary();
        if (!rhs) {
            error("Pričakovan izraz za desno stran '" + opToken.getLexem() + "' v parseMultiplicativePrime.");
            return nullptr;
        }
        auto new_lhs = std::make_unique<BinaryOpNode>(std::move(lhs), op_type, std::move(rhs), opToken.getRow(), opToken.getCol());
        return parseMultiplicativePrime(std::move(new_lhs));
    }
    return lhs; 
}

std::unique_ptr<ExprNode> Parser::parseUnary() {
    Token unaryToken = currentTokenForAst();
    UnaryOperatorType opType = UnaryOperatorType::NONE;

    if (match(Lexer::tPlus) || match(Lexer::tOnlyPlus)) {
        opType = UnaryOperatorType::PLUS;
        tryToConsume(m_currentToken.getToken());
    } else if (match(Lexer::tMinus)) {
        opType = UnaryOperatorType::MINUS;
        tryToConsume(Lexer::tMinus);
    }

    auto primaryExpr = parsePrimary();
    if (!primaryExpr) return nullptr;

    if (opType != UnaryOperatorType::NONE) {
        return std::make_unique<UnaryOpNode>(opType, std::move(primaryExpr), unaryToken.getRow(), unaryToken.getCol());
    }
    return primaryExpr;
}

std::unique_ptr<ExprNode> Parser::parsePrimary() {
    Token primaryToken = currentTokenForAst();
    if (match(Lexer::tInteger)) {
        long long val = 0;
        try {
            val = std::stoll(primaryToken.getLexem());
        } catch (const std::out_of_range&) {
            error("Integer literal izven obsega: " + primaryToken.getLexem()); return nullptr;
        } catch (const std::invalid_argument&) {
            error("Neveljaven integer literal: " + primaryToken.getLexem()); return nullptr;
        }
        tryToConsume(Lexer::tInteger);
        return std::make_unique<IntLiteralNode>(val, primaryToken.getRow(), primaryToken.getCol());
    } else if (match(Lexer::tHex)) {
        std::string hexVal = primaryToken.getLexem();
        tryToConsume(Lexer::tHex);
        return std::make_unique<HexLiteralNode>(hexVal, primaryToken.getRow(), primaryToken.getCol());
    } else if (match(Lexer::tVarLetter) || match(Lexer::tVarDigit)) {
        std::string varName = primaryToken.getLexem();
        tryToConsume(m_currentToken.getToken()); 
        return std::make_unique<VariableAccessNode>(varName, primaryToken.getRow(), primaryToken.getCol());
    } else if (match(Lexer::tCall)) {
        return parseFunctionCall();
    } else if (match(Lexer::tBracketOpen)) {
        tryToConsume(Lexer::tBracketOpen);
        auto bitwiseExpr = parseBitwise();
        if (!bitwiseExpr) return nullptr;
        if (!tryToConsume(Lexer::tBracketClosed)) {
            error("Manjka ')' v primarnem izrazu z oklepaji.");
            return nullptr;
        }
        return std::make_unique<ParenExpressionNode>(std::move(bitwiseExpr), primaryToken.getRow(), primaryToken.getCol());
    } else {
        error("Neveljaven primarni izraz (v parsePrimary).");
        return nullptr;
    }
}

// --- Commands and Functions ---
std::unique_ptr<CommandSetNode> Parser::parseCommandSet() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<CommandSetNode>(startToken.getRow(), startToken.getCol());
    auto firstCommand = parseCommand();
    if (!firstCommand) {
        return nullptr;
    }
    node->commands.push_back(std::move(firstCommand));
    if (!parseCommandSetPrime(node.get())) return nullptr;
    return node;
}

bool Parser::parseCommandSetPrime(CommandSetNode* ownerNode) {
    if (match(Lexer::tFor) || match(Lexer::tConsole) || match(Lexer::tDouble)) {
        auto nextCommand = parseCommand();
        if (!nextCommand) return false;
        ownerNode->commands.push_back(std::move(nextCommand));
        return parseCommandSetPrime(ownerNode);
    }
    return true; 
}

std::unique_ptr<CommandNode> Parser::parseCommand() {
    if (match(Lexer::tFor)) {
        return parseLoop();
    } else if (match(Lexer::tDouble)) {
        return parseAssign();
    } else if (match(Lexer::tConsole)) {
        return parsePrint();
    } else {
        error("Ukaz (Command) je lahko samo FOR, DOUBLE (Assign) ali CONSOLE.");
        return nullptr;
    }
}

std::unique_ptr<LoopNode> Parser::parseLoop() {
    Token forToken;
    if (!tryToConsume(Lexer::tFor, &forToken)) {
        error("Manjka FOR.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketOpen)) {
        error("Manjka '(' za FOR.");
        return nullptr;
    }
    auto initialization = parseAssign();
    if (!initialization) return nullptr;

    if (!tryToConsume(Lexer::tTo)) {
        error("Manjka TO v FOR zanki.");
        return nullptr;
    }
    auto to_condition = parseBitwise();
    if (!to_condition) return nullptr;
    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' v FOR zanki.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBegin)) {
        error("Manjka BEGIN v FOR zanki.");
        return nullptr;
    }
    auto body = parseBeginPrime();
    if (!body) return nullptr;
    if (!tryToConsume(Lexer::tEnd)) {
        error("Manjka END v FOR zanki.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tSemiColon)) {
        error("Manjka ';' po END v FOR zanki.");
        return nullptr;
    }
    return std::make_unique<LoopNode>(std::move(initialization), std::move(to_condition), std::move(body), forToken.getRow(), forToken.getCol());
}

std::unique_ptr<AssignNode> Parser::parseAssign() {
    Token doubleToken;
    if (!tryToConsume(Lexer::tDouble, &doubleToken)) {
        error("Manjka DOUBLE v prireditvi.");
        return nullptr;
    }
    Token varToken = currentTokenForAst();
    std::string varName;
    if (match(Lexer::tVarLetter) || match(Lexer::tVarDigit)) {
        varName = varToken.getLexem();
        tryToConsume(m_currentToken.getToken());
    } else {
        error("Manjka ime spremenljivke v prireditvi.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tEquals)) {
        error("Manjka ':=' v prireditvi.");
        return nullptr;
    }
    auto rhsExpr = parseAssignDesno();
    if (!rhsExpr) return nullptr;
    return std::make_unique<AssignNode>(varName, std::move(rhsExpr), doubleToken.getRow(), doubleToken.getCol());
}

// Assign_desno ::= Bitwise ";";
std::unique_ptr<ExprNode> Parser::parseAssignDesno() {
    auto expr = parseBitwise();
    if (!expr) return nullptr;
    if (!tryToConsume(Lexer::tSemiColon)) {
        error("Manjka ';' na koncu prireditve (v parseAssignDesno).");
        return nullptr;
    }
    return expr;
}

std::unique_ptr<PrintNode> Parser::parsePrint() {
    Token consoleToken;
    if (!tryToConsume(Lexer::tConsole, &consoleToken)) {
        error("Manjka CONSOLE.");
        return nullptr;
    }
    auto expr = parseBitwise();
    if (!expr) return nullptr;
    if (!tryToConsume(Lexer::tSemiColon)) {
        error("Manjka ';' po CONSOLE izrazu.");
        return nullptr;
    }
    return std::make_unique<PrintNode>(std::move(expr), consoleToken.getRow(), consoleToken.getCol());
}

std::unique_ptr<FunctionNode> Parser::parseFunction() {
    Token funcToken;
    if (!tryToConsume(Lexer::tFunction, &funcToken)) {
        error("Manjka FUNCTION.");
        return nullptr;
    }
    Token nameToken = currentTokenForAst();
    std::string funcName;
    if (match(Lexer::tVarLetter) || match(Lexer::tVarDigit)) {
        funcName = nameToken.getLexem();
        tryToConsume(m_currentToken.getToken());
    } else {
        error("Manjka ime funkcije.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketOpen)) {
        error("Manjka '(' za imenom funkcije.");
        return nullptr;
    }
    auto attributes = parseAtribute();
    if (!attributes) return nullptr;
    
    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' za atributi funkcije.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tCurlyBracketOpen)) {
        error("Manjka '{' za začetek telesa funkcije.");
        return nullptr;
    }
    auto body = parseBeginPrime();
    if (!body) return nullptr;
    if (!tryToConsume(Lexer::tReturn)) {
        error("Manjka RETURN v funkciji.");
        return nullptr;
    }
    auto returnExpr = parseBitwise();
    if (!returnExpr) return nullptr;
    if (!tryToConsume(Lexer::tCurlyBracketClosed)) {
        error("Manjka '}' na koncu telesa funkcije.");
        return nullptr;
    }
    return std::make_unique<FunctionNode>(funcName, std::move(attributes), std::move(body), std::move(returnExpr), funcToken.getRow(), funcToken.getCol());
}

// Atribute ::= Bitwise Atribute' | epsilon;
std::unique_ptr<AttributeNode> Parser::parseAtribute() {
    Token startToken = currentTokenForAst();
    auto node = std::make_unique<AttributeNode>(startToken.getRow(), startToken.getCol());
    
    // Preverimo, če sledi Bitwise (FIRST od Bitwise)
    if (match(Lexer::tPlus) || match(Lexer::tOnlyPlus) ||
        match(Lexer::tInteger) || match(Lexer::tMinus) ||
        match(Lexer::tHex) || match(Lexer::tVarLetter) ||
        match(Lexer::tVarDigit) || match(Lexer::tBracketOpen) ||
        match(Lexer::tCall)) {
        auto firstBitwise = parseBitwise();
        if (!firstBitwise) return nullptr;
        node->expr_list.push_back(std::move(firstBitwise));
        if (!parseAtributePrime(node.get())) return nullptr;
    }
    return node;
}

bool Parser::parseAtributePrime(AttributeNode* ownerNode) {
    if (match(Lexer::tComma)) {
        tryToConsume(Lexer::tComma);
        auto nextBitwise = parseBitwise();
        if (!nextBitwise) return false; // Napaka
        ownerNode->expr_list.push_back(std::move(nextBitwise));
        return parseAtributePrime(ownerNode);
    }
    return true; 
}

std::unique_ptr<FunctionCallNode> Parser::parseFunctionCall() {
    Token callToken;
    if (!tryToConsume(Lexer::tCall, &callToken)) {
        error("Manjka CALL.");
        return nullptr;
    }
    Token nameToken = currentTokenForAst();
    std::string funcName;
    if (match(Lexer::tVarLetter) || match(Lexer::tVarDigit)) {
        funcName = nameToken.getLexem();
        tryToConsume(m_currentToken.getToken());
    } else {
        error("Manjka ime funkcije pri klicu.");
        return nullptr;
    }
    if (!tryToConsume(Lexer::tBracketOpen)) {
        error("Manjka '(' za imenom klicane funkcije.");
        return nullptr;
    }
    auto attributes = parseAtribute();
    if (!attributes) return nullptr;

    if (!tryToConsume(Lexer::tBracketClosed)) {
        error("Manjka ')' za argumenti klica funkcije.");
        return nullptr;
    }
    return std::make_unique<FunctionCallNode>(funcName, std::move(attributes), callToken.getRow(), callToken.getCol());
}