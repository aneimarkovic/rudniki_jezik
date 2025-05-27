#include "Parser.hpp"
#include <fstream>
#include <sstream>

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
        return true; 
    }

    try {
        while (!m_currentToken.isEof()) 
        {
            if (!this->parseBegin()) {         
                return false;
            }
        }
        return true; 
    }
    catch (...) {
        return false;
    }
}

/*
    Funkcija, ki se uporablja za izpisvanje napak
*/
void Parser::error(std::string errorMessage) 
{
    fprintf(stderr, "[ERROR] %d:%d >> %s!\n", m_currentToken.getRow(), m_currentToken.getCol(), errorMessage.c_str());
}

/*
    Preveri ali je [m_currentToken] pravilne vrste, če je in vrne true
*/
bool Parser::match(int expectedType) {
    if (m_currentToken.getToken() == expectedType) {
        return true;
    }

    return false;
}

/*
    Premakne na naslednji token
*/
bool Parser::tryToConsume(int expectedType)
{
    if (m_currentToken.getToken() == expectedType) {
        m_currentToken = m_lexer.nextToken();
        return true;
    }

    return false;
}

/*
     
    Begin ::= Start
        | Function
        | Assign CommandSet
        | Non_Assign_Command CommandSet
        | Expr;
*/
bool Parser::parseBegin()
{
    if (match(Lexer::tMine))
    {
        return parseStart();
    }
    else if (match(Lexer::tFunction))
    {
        return parseFunction();
    }
    else if (match(Lexer::tDouble)) 
    {
        return parseAssign();
    }
    else if (match(Lexer::tFor) || match(Lexer::tConsole))  
    {
        return parseNonAssignCommand();
    }
    else if (match(Lexer::tPlus) 
        || match(Lexer::tMinus)
        || match(Lexer::tHex)
        || match(Lexer::tVarLetter)
        || match(Lexer::tVarDigit)
        || match(Lexer::tBracketOpen)
        || match(Lexer::tCall))
    {
        return parseExpr();
    }

    error("Nepravilen začetek programa ali bloka."); 
    return false;
}

/*
    Logika za: BeginPrime ::= CommandSet | Bitwise | epsilon;
*/
bool Parser::parseBeginPrime()
{
    if (match(Lexer::tFor) 
        || match(Lexer::tDouble) 
        || match(Lexer::tConsole))
    {
        return parseCommandSet();
    }
    else if (match(Lexer::tPlus)
        || match(Lexer::tInteger)
        || match(Lexer::tMinus)
        || match(Lexer::tHex)
        || match(Lexer::tVarLetter)
        || match(Lexer::tVarDigit)
        || match(Lexer::tBracketOpen)
        || match(Lexer::tCall))
    {
        return parseBitwise();
    }
    else
    {
        return true;
    }
}

/*
    Logika za: Non_Assign_Command ::= Loop | Print;
*/
bool Parser::parseNonAssignCommand()
{
    if (match(Lexer::tFor))
    {
        return parseLoop();
    }
    else if (match(Lexer::tConsole))
    {
        return parsePrint();
    }
    else
    {
        error("Manjka Loop ali Print");
        return false;
    }
}

/*
     Start ::= TOKEN_MINE STRING "{" Minetype "}";
*/
bool Parser::parseStart()
{
    if (!tryToConsume(Lexer::tMine))
    {
        error("Manjka MINE");
        return false;
    }

    if (!tryToConsume(Lexer::tVarLetter) && !tryToConsume(Lexer::tVarDigit))
    {
        error("Manjka ime rudnika");
        return false;
    }

    if (!tryToConsume(Lexer::tCurlyBracketOpen))
    {
        error("Manjka \"{\"");
        return false;
    }
    
    bool returnValue;
    returnValue = parseMinetype();

    if (!tryToConsume(Lexer::tCurlyBracketClosed))
    {
        error("Manjka \"}\"");
        return false;
    }

    return returnValue;
}

/*
    Logika za
    Minetype ::= SURFACE "{" "[" Landelements "]" "}"
           | UNDERGROUND "{" "[" Undergroundelements "]" "}"
           | IN_PLACE "{" "[" Landelements "]" "}"
           | IN_SITU "{" "[" Landelements "]" "}"
           | DEEP_SEA "{" "[" Seaelements "]" "}";
*/
bool Parser::parseMinetype()
{
    int mineType = m_currentToken.getToken();
    if (!tryToConsume(Lexer::tSurface)
        && !tryToConsume(Lexer::tUnderground)
        && !tryToConsume(Lexer::tInplace)
        && !tryToConsume(Lexer::tInsitu)
        && !tryToConsume(Lexer::tDeepsea))
    {
        error("Manjka tip rudnika");
        return false;
    }

    if (!tryToConsume(Lexer::tCurlyBracketOpen))
    {
        error("Manjka \"{\"");
        return false;
    }

    if (!tryToConsume(Lexer::tSquareBracketOpen))
    {
        error("Manjka \"[\"");
        return false;
    }

    bool returnValue;
    if (mineType == Lexer::tSurface
        || mineType == Lexer::tInplace
        || mineType == Lexer::tInsitu)
    {
        returnValue = parseLandelements();
    }
    else if (mineType == Lexer::tUnderground)
    {
        returnValue = parseUndergroundelements();
    }
    else if (mineType == Lexer::tDeepsea)
    {
        returnValue = parseSeaelements();
    }
    else
    {
        error("Napaka pri branju rudnika");
        return false;
    }

    if (!tryToConsume(Lexer::tSquareBracketClosed))
    {
        error("Manjka \"]\"");
        return false;
    }

    if (!tryToConsume(Lexer::tCurlyBracketClosed))
    {
        error("Manjka \"}\"");
        return false;
    }

    return returnValue;
}

/*
     Landelements ::= Landelements_Element Landelements';
*/
bool Parser::parseLandelements()
{
    return parseLandelementsElement() && parseLandelementsPrime();
}

/*
     Landelements' ::= "," Landelements_Element Landelements' | ε;
*/
bool Parser::parseLandelementsPrime()
{
    if (!tryToConsume(Lexer::tComma))
    {
        return true;
    }

    return parseLandelementsElement() && parseLandelementsPrime();
}

/*
     Undergroundelements ::= Undergroundelements_Element Undergroundelements';
*/
bool Parser::parseUndergroundelements()
{
    return parseUndergroundelementsElement() && parseUndergroundelementsPrime();
}

/*
     Undergroundelements' ::= "," Undergroundelements_Element Undergroundelements' | ε;
*/
bool Parser::parseUndergroundelementsPrime()
{
    if (!tryToConsume(Lexer::tComma))
    {
        return true;
    }

    return parseUndergroundelementsElement() && parseUndergroundelementsPrime();
}

/*
     Seaelements :: = Seaelements_Element Seaelements';
*/
bool Parser::parseSeaelements()
{
    return parseSeaelementsElement() && parseSeaelementsPrime();
}

/*
     Seaelements' ::= "," Seaelements_Element Seaelements' | ε;
*/
bool Parser::parseSeaelementsPrime()
{
    if (!tryToConsume(Lexer::tComma))
    {
        return true;
    }

    return parseSeaelementsElement() && parseSeaelementsPrime();
}

/*
     Landelements_Element ::= "{" Landelements_Curly_Choice "}" | "[" Landelements_Bracket_Choice "]";
*/
bool Parser::parseLandelementsElement()
{
    if (match(Lexer::tCurlyBracketOpen)) 
    {
        tryToConsume(Lexer::tCurlyBracketOpen); 
        if (!parseLandelementsCurlyChoice())
        {
            return false;
        }
        if (!tryToConsume(Lexer::tCurlyBracketClosed)) 
        {
            error("Manjka \"}\"");
            return false;
        }
        return true; 
    }
    else if (match(Lexer::tSquareBracketOpen)) 
    {
        tryToConsume(Lexer::tSquareBracketOpen); 
        if (!parseLandelementsBracketChoice())
        {
            return false;
        }
        if (!tryToConsume(Lexer::tSquareBracketClosed)) 
        {
            error("Manjka \"]\"");
            return false;
        }
        return true; 
    }
    else
    {
        error("Manjka \"{\" ali \"[\" na začetku");
        return false; 
    }
}

/*
    Landelements_Curly_Choice ::= TOKEN_PATH Path_Vsebina
                            |  TOKEN_RIVER River_Vsebina;
*/
bool Parser::parseLandelementsCurlyChoice()
{
    if (tryToConsume(Lexer::tPath))
    {
        return parsePathVsebina();
    }
    else if (tryToConsume(Lexer::tRiver))
    {
        return parseRiverVsebina();
    }
    else
    {
        error("Podpira smao Path, River");
        return false;
    }
}

/*
    Path_Vsebina ::= Line Lines | Polyline Lines | Bend Lines;
*/
bool Parser::parsePathVsebina()
{
    if (match(Lexer::tLine))
    {
        if (!parseLine()) return false;
    }
    else if (match(Lexer::tPolyline))
    {
        if (!parsePolyline()) return false;
    }
    else if (match(Lexer::tBend))
    {
        if (!parseBend()) return false;
    }
    else
    {
        error("Path podpira samo Line, Polyline in Bend");
        return false;
    }

    return parseLines();
}

/*
    River_Vsebina ::=  Line Lines | Polyline Lines | Bend Lines;
*/
bool Parser::parseRiverVsebina()
{
    bool returnValue = false;

    if (match(Lexer::tLine))
    {
        if (!parseLine()) return false;
    }
    else if (match(Lexer::tPolyline))
    {
        if (!parsePolyline()) return false;
    }
    else if (match(Lexer::tBend))
    {
        if (!parseBend()) return false;
    }
    else
    {
        error("River podpira samo Line, Polyline in Bend");
        return false;
    }

    return parseLines();
}

/*
    
Landelements_Bracket_Choice ::= TOKEN_STRIP Strip_Vsebina
                            |  TOKEN_BENCH Bench_Vsebina
                            |  TOKEN_SPOILPIT Spoilpit_Vsebina
                            |  TOKEN_PLANT Plant_Vsebina
                            |  TOKEN_WELL Well_Vsebina;
*/
bool Parser::parseLandelementsBracketChoice()
{
    if (tryToConsume(Lexer::tStrip))
    {
        return parseStripVsebina();
    }
    else if (tryToConsume(Lexer::tBench))
    {
        return parseBenchVsebina();
    }
    else if (tryToConsume(Lexer::tSpoilpit))
    {
        return parseSpoilpitVsebina();
    }
    else if (tryToConsume(Lexer::tPlant))
    {
        return parsePlantVsebina();
    }
    else if (tryToConsume(Lexer::tWell))
    {
        return parseWellVsebina();
    }
    else
    {
        error("Podprt samo Strip, Bench, Spoilpit in Well");
        return false;
    }
}

/*
    Strip_Vsebina ::= Arbitarraypolygon | Box;
*/
bool Parser::parseStripVsebina()
{
    if (match(Lexer::tPolygon))
    {
        return parseArbitarraypolygon();
    }
    else if (match(Lexer::tBox))
    {
        return parseBox();
    }
    else
    {
        error("Strip podpira samo Polygon in Box");
        return false;
    }
}

/*
    Bench_Vsebina ::= Arbitarraypolygon;
*/
bool Parser::parseBenchVsebina()
{
    if (match(Lexer::tPolygon))
    {
        return parseArbitarraypolygon();
    }
    else
    {
        error("Beach podpira samo Polygon");
        return false;
    }
}

/*
    Spoilpit_Vsebina ::= Arbitarraypolygon | Box | Circ;
*/
bool Parser::parseSpoilpitVsebina()
{
    if (match(Lexer::tPolygon))
    {
        return parseArbitarraypolygon();
    }
    else if (match(Lexer::tBox))
    {
        return parseBox();
    }
    else if (match(Lexer::tCircle))
    {
        return parseCirc();
    }
    else
    {
        error("Spoilpit podpira samo Polygon, Box in Circ");
        return false;
    }
}

/*
    Plant_Vsebina ::= Arbitarraypolygon | Box | Circ;
*/
bool Parser::parsePlantVsebina()
{
    if (match(Lexer::tPolygon))
    {
        return parseArbitarraypolygon();
    }
    else if (match(Lexer::tBox))
    {
        return parseBox();
    }
    else if (match(Lexer::tCircle))
    {
        return parseCirc();
    }
    else
    {
        error("Plant podpira samo Polygon, Box in Circ");
        return false;
    }
}

/*
    Well_Vsebina ::= WELLTYPE Point;
*/
bool Parser::parseWellVsebina()
{
    if (!tryToConsume(Lexer::tMonitoring) &&
        !tryToConsume(Lexer::tInjection) &&
        !tryToConsume(Lexer::tRecovery)) {
        error("Well mora biti tipa MONITORING, INJECTION ali RECOVERY");
        return false;
    }
    if (!parsePoint()) {
        error("Manjka Point po WELLTYPE v Well_Vsebina");
        return false;
    }
    return true;
}

/*
     Undergroundelements_Element ::= "{" Undergroundelements_Curly_Choice "}" | '[' Undergroundelements_Bracket_Choice ']';
*/
bool Parser::parseUndergroundelementsElement()
{
   if (match(Lexer::tCurlyBracketOpen)) 
    {
        tryToConsume(Lexer::tCurlyBracketOpen); 
        if (!parseUndergroundelementsCurlyChoice())
        {
            return false;
        }
        if (!tryToConsume(Lexer::tCurlyBracketClosed)) 
        {
            error("Manjka \"}\"");
            return false;
        }
        return true; 
    }
    else if (match(Lexer::tSquareBracketOpen)) 
    {
        tryToConsume(Lexer::tSquareBracketOpen); 
        if (!parseUndergroundelementsBracketChoice())
        {
            return false;
        }
        if (!tryToConsume(Lexer::tSquareBracketClosed)) 
        {
            error("Manjka \"]\"");
            return false;
        }
        return true; 
    }
    else
    {
        error("Manjka \"{\" ali \"[\" na začetku");
        return false; 
    }
}

/*
    Undergroundelements_Curly_Choice ::= TOKEN_ELEVATOR Elevator_Vsebina
                                    | TOKEN_OREBED Orebed_Vsebina;
*/
bool Parser::parseUndergroundelementsCurlyChoice()
{
    if (tryToConsume(Lexer::tElevator))
    {
        return parseElevatorVsebina();
    }
    else if (tryToConsume(Lexer::tOreBed))
    {
        return parseOrebedVsebina();
    }
    else
    {
        error("Podpira samo Elevator in Orebed");
        return false;
    }
}

/*
    Elevator_Vsebina ::= ELEVATORTYPE | Point;
*/

bool Parser::parseElevatorVsebina()
{
    int currentType = m_currentToken.getToken();

    if (currentType == Lexer::tHuman || currentType == Lexer::tOre)
    {
        if (tryToConsume(Lexer::tHuman) || tryToConsume(Lexer::tOre))
        {
            return true;
        }
        else
        {
            error("Interna napaka: Pričakovan ELEVATORTYPE");
            return false;
        }
    }
    else if (currentType == Lexer::tPoint)
    {
        return parsePoint();
    }
    else
    {
        error("Elevator podpira samo ELEVATORTYPE (human, ore) ali Point");
        return false;
    }
}

/*
    Orebed_Vsebina ::= Arbitarraypolygon | Box | Circ;
*/
bool Parser::parseOrebedVsebina()
{
    int currentType = m_currentToken.getToken();

    if (currentType == Lexer::tPolygon)
    {
        return parseArbitarraypolygon();
    }
    else if (currentType == Lexer::tBox)
    {
        return parseBox();
    }
    else if (currentType == Lexer::tCircle)
    {
        return parseCirc();
    }
    else
    {
        error("Orebed podpira samo Polygon, Box ali Circ");
        return false;
    }
}

/*
    Undergroundelements_Bracket_Choice ::= TOKEN_TUNNEL Tunnel_Vsebina
                                        |  TOKEN_SHAFT Shaft_Vsebina;
*/
bool Parser::parseUndergroundelementsBracketChoice()
{
    if (tryToConsume(Lexer::tTunnel))
    {
        return parseTunnelVsebina();
    }
    else if (tryToConsume(Lexer::tShaft))
    {
        return parseShaftVsebina();
    }
    else
    {
        error("Undergroundelements podpira samo Tunnel ali Shaft");
        return false;
    }
}

/*
    Tunnel_Vsebina ::= Polyline Lines | Line Lines | Bend Lines;
*/
bool Parser::parseTunnelVsebina()
{
    bool shapeParsed = false;
    int currentType = m_currentToken.getToken();

    if (currentType == Lexer::tPolyline)
    {
        shapeParsed = parsePolyline();
    }
    else if (currentType == Lexer::tLine)
    {
        shapeParsed = parseLine();
    }
    else if (currentType == Lexer::tBend)
    {
        shapeParsed = parseBend();
    }
    else
    {
        error("Tunnel podpira samo Polyline, Line ali Bend");
        return false;
    }

    if (!shapeParsed)
    {
        return false;
    }

    return parseLines();
}

/*
    Shaft_Vsebina ::=  Polyline Lines | Line Lines | Bend Lines;
*/
bool Parser::parseShaftVsebina()
{
    bool shapeParsed = false;
    int currentType = m_currentToken.getToken();

    if (currentType == Lexer::tPolyline)
    {
        shapeParsed = parsePolyline();
    }
    else if (currentType == Lexer::tLine)
    {
        shapeParsed = parseLine();
    }
    else if (currentType == Lexer::tBend)
    {
        shapeParsed = parseBend();
    }
    else
    {
        error("Shaft podpira samo Polyline, Line ali Bend");
        return false;
    }

    return shapeParsed && parseLines();
}

/*
     Seaelements_Element ::= "{" Seaelements_Curly_Choice "}";
*/
bool Parser::parseSeaelementsElement()
{
    if (!tryToConsume(Lexer::tCurlyBracketOpen))
    {
        error("Manjka \"{\"");
        return false;
    }

    if (!parseSeaelementsCurlyChoice())
    {
        return false;
    }

    if (!tryToConsume(Lexer::tCurlyBracketClosed))
    {
        error("Manjka \"}\"");
        return false;
    }
    return true;
}

/*
    Seaelements_Curly_Choice ::=  TOKEN_PLATFORM Platform_Vsebina
                                | TOKEN_PUMP Pump_Vsebina
                                | TOKEN_RISER Riser_Vsebina;
*/
bool Parser::parseSeaelementsCurlyChoice()
{
    if (tryToConsume(Lexer::tPlatform))
    {
        return parsePlatformVsebina();
    }
    else if (tryToConsume(Lexer::tPump))
    {
        return parsePumpVsebina();
    }
    else if (tryToConsume(Lexer::tRiser))
    {
        return parseRiserVsebina();
    }
    else
    {
        error("Seaelements podpira samo Platform, Pump ali Riser");
        return false;
    }
}

/*
    Platform_Vsebina ::= Arbitarraypolygon | Box | Circ;
*/
bool Parser::parsePlatformVsebina()
{
    int currentType = m_currentToken.getToken();

    if (currentType == Lexer::tPolygon)
    {
        return parseArbitarraypolygon();
    }
    else if (currentType == Lexer::tBox)
    {
        return parseBox();
    }
    else if (currentType == Lexer::tCircle)
    {
        return parseCirc();
    }
    else
    {
        error("Platform podpira samo Polygon, Box ali Circ");
        return false;
    }
}

/*
    Pump_Vsebina ::= Point;
*/
bool Parser::parsePumpVsebina()
{
    return parsePoint();
}

/*
    Riser_Vsebina ::= Point;
*/
bool Parser::parseRiserVsebina()
{
    return parsePoint();
}

/*
    Lines ::= "," "{" STRING Lines_Content "}" Lines | ε;
*/
bool Parser::parseLines()
{
    if (!tryToConsume(Lexer::tComma))
    {
        return true;
    }

    if (!tryToConsume(Lexer::tCurlyBracketOpen))
    {
        error("Manjka \"{\"");
        return false;
    }

    if (!tryToConsume(Lexer::tVarLetter) && !tryToConsume(Lexer::tVarDigit))
    {
        error("Manjka STRING");
        return false;
    }

    if (!parseLinesContent()) return false;

    if (!tryToConsume(Lexer::tCurlyBracketClosed))
    {
        error("Manjka \"}\"");
        return false;
    }

    return parseLines();
}

/*
    Lines_Content ::= "(" Lines_Item_Content_After_Paren ")";
*/
bool Parser::parseLinesContent()
{
    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!parseLinesItemContentAfterParen()) return false;

    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    return true;
}

/*
    Lines_Item_Content_After_Paren ::= "[" Points "]"
                                 | Point "," Point Lines_Bend_Opt;
*/
bool Parser::parseLinesItemContentAfterParen()
{
    if (tryToConsume(Lexer::tSquareBracketOpen))
    {
        if (!parsePoints()) return false;
        if (!tryToConsume(Lexer::tSquareBracketClosed))
        {
            error("Manjka \"]\"");
            return false;
        }

        return true;
    }
    else if (match(Lexer::tPoint))
    {
        if (!parsePoint()) return false;

        if (!tryToConsume(Lexer::tComma))
        {
            error("Manjka \",\"");
            return false;
        }

        if (!parsePoint()) return false;
        if (!parseLinesBendOpt()) return false;

        return true;
    }
    else
    {
        error("Neznan Token");
        return false;
    }
}

/*
    Lines_Bend_Opt ::= "," Bitwise | ε;
*/
bool Parser::parseLinesBendOpt()
{
    if (!tryToConsume(Lexer::tComma))
    {
        return true;
    }

    return parseBitwise();
}

/*
    Arbitarraypolygon ::= TOKEN_POLYGON "(" "[" Points "]" ")";
*/
bool Parser::parseArbitarraypolygon()
{
    if (!tryToConsume(Lexer::tPolygon))
    {
        error("Manjka POLYGON");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!tryToConsume(Lexer::tSquareBracketOpen))
    {
        error("Manjka \"[\"");
        return false;
    }

    if (!parsePoints()) { return false; }

    if (!tryToConsume(Lexer::tSquareBracketClosed))
    {
        error("Manjka \"]\"");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    return true;
}

/*
    Polyline ::= TOKE_POLYLINE "(" "[" Points "]" ")";
*/

bool Parser::parsePolyline()
{
    if (!tryToConsume(Lexer::tPolyline))
    {
        error("Manjka POLYLINE");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!tryToConsume(Lexer::tSquareBracketOpen))
    {
        error("Manjka \"[\"");
        return false;
    }

    if (!parsePoints()) return false;

    if (!tryToConsume(Lexer::tSquareBracketClosed))
    {
        error("Manjka \"]\"");
        return false;
    }


    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    return true;
}

/*
    Line ::= TOKEN_LINE "(" Point "," Point ")";
*/
bool Parser::parseLine()
{

    if (!tryToConsume(Lexer::tLine))
    {
        error("Manjka LINE");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!parsePoint())  return false; 

    if (!tryToConsume(Lexer::tComma))
    {
        error("Manjka \",\"");
        return false;
    }

    if (!parsePoint())  return false; 

    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    return true;
}

/*
    Box ::= TOKEN_BOX "(" Point "," Point ")";
*/
bool Parser::parseBox()
{
    if (!tryToConsume(Lexer::tBox))
    {
        error("Manjka BOX");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!parsePoint())  return false;

    if (!tryToConsume(Lexer::tComma))
    {
        error("Manjka \",\"");
        return false;
    }

    if (!parsePoint())  return false;

    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    return true;
}

/*
    Points ::= Point Points';
*/
bool Parser::parsePoints()
{
    if (!parsePoint()) return false;
    return parsePointsPrime();
}

/*
    Points' ::= "," Point Points' | ε;
*/
bool Parser::parsePointsPrime()
{
    if (!tryToConsume(Lexer::tComma))
    {
        return true;
    }

    return parsePoint() && parsePointsPrime();
}

/*
    Point ::= TOKEN_POINT "(" Bitwise "," Bitwise ")";
*/
bool Parser::parsePoint()
{
    if (!tryToConsume(Lexer::tPoint))
    {
        error("Manjka POINT");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!parseBitwise()) return false;

    if (!tryToConsume(Lexer::tComma))
    {
        error("Manjka \",\"");
        return false;
    }

    if (!parseBitwise()) return false;

    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    return true;
}

/*
    Bend ::= TOKEN_BEND "(" Point "," Point "," Bitwise ")";
*/
bool Parser::parseBend()
{
    if (!tryToConsume(Lexer::tBend))
    {
        error("Manjka BEND");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!parsePoint()) return false;

    if (!tryToConsume(Lexer::tComma))
    {
        error("Manjka \",\"");
        return false;
    }

    if (!parsePoint()) return false;

    if (!tryToConsume(Lexer::tComma))
    {
        error("Manjka \",\"");
        return false;
    }

    if (!parseBitwise()) return false;

    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    return true;
}

/*
    Circ ::= TOKEN_CIRC "(" Point "," Bitwise ")";
*/
bool Parser::parseCirc()
{
    if (!tryToConsume(Lexer::tCircle))
    {
        error("Manjka CIRC");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!parsePoint()) return false;

    if (!tryToConsume(Lexer::tComma))
    {
        error("Manjka \",\"");
        return false;
    }

    if (!parseBitwise()) return false;

    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    return true;
}


/*
    Expr ::= Bitwise;
*/
bool Parser::parseExpr()
{
    return parseBitwise();
}

/*
    Bitwise ::= Additive Bitwise';
*/

bool Parser::parseBitwise()
{
    return (parseAdditive() && parseBitwisePrime());
}

/*
    Bitwise' ::= bwand Additive Bitwise' | bwor Additive Bitwise' | ε;
*/
bool Parser::parseBitwisePrime()
{
    if (tryToConsume(Lexer::tAND))
    {
        return (parseAdditive() && parseBitwisePrime());
    }
    else if (tryToConsume(Lexer::tOR))
    {
        return (parseAdditive() && parseBitwisePrime());
    }
    else
    {
        return true;
    }
}

/*
    Additive ::= Multiplicative Additive';
*/
bool Parser::parseAdditive()
{
    return (parseMultiplicative() && parseAdditivePrime());
}

/*
    Additive' ::= plus Multiplicative Additive' | minus Multiplicative Additive' | ε;
*/
bool Parser::parseAdditivePrime()
{
    if (tryToConsume(Lexer::tPlus) || tryToConsume(Lexer::tOnlyPlus))
    {
        return (parseMultiplicative() && parseAdditivePrime());
    }
    else if (tryToConsume(Lexer::tMinus))
    {
        return (parseMultiplicative() && parseAdditivePrime());
    }
    else
    {
        return true;
    }
}

/*
    Multiplicative ::= Unary Multiplicative';
*/
bool Parser::parseMultiplicative()
{
    return (parseUnary() && parseMultiplicativePrime());
}

/*
    Multiplicative' ::= times Unary Multiplicative' | divide Unary Multiplicative' | ε;
*/
bool Parser::parseMultiplicativePrime()
{
    if (tryToConsume(Lexer::tTimes) || tryToConsume(Lexer::tOnlyTimes))
    {
        return (parseUnary() && parseMultiplicativePrime());
    }
    else if (tryToConsume(Lexer::tSlash))
    {
        return (parseUnary() && parseMultiplicativePrime());
    }
    else
    {
        return true;
    }
}

/*
    Unary ::= plus Primary | minus Primary | Primary;
*/
bool Parser::parseUnary()
{
    if (tryToConsume(Lexer::tPlus))
    {
        return parsePrimary();
    }
    else if (tryToConsume(Lexer::tMinus))
    {
        return parsePrimary();
    }

    return parsePrimary();
}

/*
    Primary ::= INT 
            | hex
          | variable
          | FunctionCall
          | "(" Bitwise ")";
*/
bool Parser::parsePrimary()
{

    if (tryToConsume(Lexer::tInteger))
    {
        return true;
    }
    else if (tryToConsume(Lexer::tHex))
    {
        return true;
    }
    else if (tryToConsume(Lexer::tVarLetter) || tryToConsume(Lexer::tVarDigit))
    {
        return true;
    }
    else if (match(Lexer::tCall))
    {
        return parseFunctionCall();
    }
    else if (tryToConsume(Lexer::tBracketOpen))
    {
        if (!parseBitwise()) return false;

        if (!tryToConsume(Lexer::tBracketClosed))
        {
            error("Manjka \")\"");
            return false;
        }
    }
    else
    {
        error("Invalid primary");
        return false;
    }
}

/*
    CommandSet ::= Command CommandSet';
*/
bool Parser::parseCommandSet()
{
    return (parseCommand() && parseCommandSetPrime());
}

/*
    CommandSet' ::= Command CommandSet' | ε;
*/
bool Parser::parseCommandSetPrime()
{
    if (match(Lexer::tFor)
        || match(Lexer::tConsole)
        || match(Lexer::tDouble))
    {
        return (parseCommand() && parseCommandSetPrime());
    }
    else
    {
        return true;
    }
}

/*
    Command ::= Loop | Assign | Print;
*/
bool Parser::parseCommand()
{
    if (match(Lexer::tFor))
    {
        return parseLoop();
    }
    else if (match(Lexer::tDouble))
    {
        return parseAssign();
    }
    else if (match(Lexer::tConsole))
    {
        return parsePrint();
    }
    else
    {
        error("Command lahko ima samo for, assing ali console");
        return false;
    }
}

/*
    Loop ::= "for" "(" Assign "to" Bitwise ")" "begin" BeginPrime "end;";
*/
bool Parser::parseLoop()
{
    if (!tryToConsume(Lexer::tFor))
    {
        error("Manjka FOR");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!parseAssign()) return false;

    if (!tryToConsume(Lexer::tTo))
    {
        error("Manjka TO");
        return false;
    }

    if (!parseBitwise()) return false;

    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    if (!tryToConsume(Lexer::tBegin))
    {
        error("Manjka BEGIN");
        return false;
    }

    if (!parseBeginPrime()) return false;

    if (!tryToConsume(Lexer::tEnd))
    {
        error("Manjka END");
        return false;
    }

    if (!tryToConsume(Lexer::tSemiColon))
    {
        error("Manjka \";\"");
        return false;
    }

    return true;
}

/*
    Assign ::= TOKEN_DOUBLE variable ":=" Assign_desno;
*/
bool Parser::parseAssign()
{
    if (!tryToConsume(Lexer::tDouble))
    {
        error("Manjka DOUBLE");
        return false;
    }

    if (!tryToConsume(Lexer::tVarLetter) && !tryToConsume(Lexer::tVarDigit))
    {
        error("Manjka ime spremenljivke");
        return false;
    }

    if (!tryToConsume(Lexer::tEquals))
    {
        error("Manjka \":=\"");
        return false;
    }
    
    if (!parseAssignDesno()) return false;

    return true;
}

/*
    Assign_desno ::= Bitwise ";";
*/
bool Parser::parseAssignDesno()
{
    if (!parseBitwise()) return false;

    if (!tryToConsume(Lexer::tSemiColon))
    {
        error("Manjka \";\"");
        return false;
    }

    return true;
}

/*
    Print ::= "CONSOLE" Bitwise ";";
*/
bool Parser::parsePrint()
{
    if (!tryToConsume(Lexer::tConsole))
    {
        error("Manjka CONSOLE");
        return false;
    }

    if (!parseBitwise()) return false;

    if (!tryToConsume(Lexer::tSemiColon))
    {
        error("Manjka \";\"");
        return false;
    }

    return true;
}

/*
    Function ::= TOKEN_FUNCTION STRING "(" Atribute ")" "{" BeginPrime return Bitwise "}";
*/
bool Parser::parseFunction()
{
    if (!tryToConsume(Lexer::tFunction))
    {
        error("Manjka FUNCTION");
        return false;
    }

    if (!tryToConsume(Lexer::tVarLetter) && !tryToConsume(Lexer::tVarDigit))
    {
        error("Manjka ime funkcije");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!parseAtribute()) return false;

    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    if (!tryToConsume(Lexer::tCurlyBracketOpen))
    {
        error("Manjka \"{\"");
        return false;
    }

    if (!parseBeginPrime()) return false;

    if (!tryToConsume(Lexer::tReturn))
    {
        error("Manjka RETURN");
        return false;
    }

    if (!parseBitwise()) return false;

    if (!tryToConsume(Lexer::tCurlyBracketClosed))
    {
        error("Manjka \"}\"");
        return false;
    }

    return true;
}

/*
    Atribute ::= Bitwise Atribute' | epsilon; 
*/
bool Parser::parseAtribute()
{
    if (match(Lexer::tPlus)
        || match(Lexer::tInteger)
        || match(Lexer::tMinus)
        || match(Lexer::tHex)
        || match(Lexer::tVarLetter)
        || match(Lexer::tVarDigit)
        || match(Lexer::tBracketOpen)
        || match(Lexer::tCall))
    {
        if (!parseBitwise()) {
            return false;
        }
        if (!parseAtributePrime()) {
            return false;
        }
        return true; 
    }
    else {
        return true;
    }
}

/*
    Atribute' ::= "," Bitwise Atribute' | ε;
*/
bool Parser::parseAtributePrime()
{
    if (!tryToConsume(Lexer::tComma))
    {
        return true;
    }

    return parseBitwise() && parseAtributePrime();
}

/*
    FunctionCall ::= "call" STRING "(" Atribute ")";
*/
bool Parser::parseFunctionCall()
{
    if (!tryToConsume(Lexer::tCall))
    {
        error("Manjka CALL");
        return false;
    }

    if (!tryToConsume(Lexer::tVarLetter) && !tryToConsume(Lexer::tVarDigit))
    {
        error("Manjka ime funkcije");
        return false;
    }

    if (!tryToConsume(Lexer::tBracketOpen))
    {
        error("Manjka \"(\"");
        return false;
    }

    if (!parseAtribute()) return false;

    if (!tryToConsume(Lexer::tBracketClosed))
    {
        error("Manjka \")\"");
        return false;
    }

    return true;
}