// Created by Anei Markovič 19.05.2024
#include "Lexer.hpp"
//Metoda ustvari automat oz. tabelo povezav
void Lexer::initAutomata()
{
    for (int i = 0; i <= maxState; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            automata[i][j] = noEdge;
        }
    }

    automata[0]['F'] = 19;
    automata[19]['O'] = 20;
    automata[20]['R'] = 21;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'O')
        {
            automata[19][i] = 4;
        }
    }

    automata[0]['B'] = 22;
    automata[22]['E'] = 23;

    automata[23]['G'] = 24;
    automata[24]['I'] = 25;
    automata[25]['N'] = 26;

    automata[23]['N'] = 42;
    automata[42]['D'] = 43;

    automata[42]['C'] = 44;
    automata[44]['H'] = 45;

    automata[22]['O'] = 46;
    automata[46]['X'] = 47;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'E' || i != 'N' || i != 'O')
        {
            automata[22][i] = 4;
        }
    }

    automata[0]['E'] = 27;
    automata[27]['N'] = 28;
    automata[28]['D'] = 29;

    automata[27]['L'] = 56;
    automata[56]['E'] = 57;
    automata[57]['V'] = 58;
    automata[58]['A'] = 59;
    automata[59]['T'] = 60;
    automata[60]['O'] = 61;
    automata[61]['R'] = 62;

    for (int i = 'a'; i <= 'Z'; i++)
    {
        if (i != 'N' || i != 'L')
        {
            automata[27][i] = 4;
        }
    }

    automata[0]['C'] = 30;
    automata[30]['O'] = 31;
    automata[31]['N'] = 32;
    automata[32]['S'] = 33;
    automata[33]['O'] = 34;
    automata[34]['L'] = 35;
    automata[35]['E'] = 36;

    automata[30]['A'] = 48;
    automata[48]['L'] = 49;
    automata[49]['L'] = 50;

    automata[30]['I'] = 51;
    automata[51]['R'] = 52;
    automata[52]['C'] = 53;
    automata[53]['L'] = 54;
    automata[54]['E'] = 55;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'O' || i != 'A' || i != 'I')
        {
            automata[30][i] = 4;
        }
    }

    automata[0]['D'] = 63;
    automata[63]['E'] = 64;
    automata[64]['E'] = 65;
    automata[65]['P'] = 66;
    automata[66]['S'] = 67;
    automata[67]['E'] = 68;
    automata[68]['A'] = 69;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'E')
        {
            automata[63][i] = 4;
        }
    }

    automata[0]['H'] = 70;
    automata[70]['U'] = 71;
    automata[71]['M'] = 72;
    automata[72]['A'] = 73;
    automata[73]['N'] = 74;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'H')
        {
            automata[70][i] = 4;
        }
    }

    automata[0]['I'] = 75;
    automata[75]['N'] = 76;

    automata[76]['P'] = 77;
    automata[77]['L'] = 78;
    automata[78]['A'] = 79;
    automata[79]['C'] = 80;
    automata[80]['E'] = 81;

    automata[76]['S'] = 82;
    automata[82]['I'] = 83;
    automata[83]['T'] = 84;
    automata[84]['U'] = 85;

    automata[76]['J'] = 86;
    automata[86]['E'] = 87;
    automata[87]['C'] = 88;
    automata[88]['T'] = 89;
    automata[89]['I'] = 90;
    automata[90]['O'] = 91;
    automata[91]['N'] = 92;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'N')
        {
            automata[75][i] = 4;
        }
    }

    automata[0]['L'] = 93;
    automata[93]['I'] = 94;
    automata[94]['N'] = 95;
    automata[95]['E'] = 96;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'I')
        {
            automata[93][i] = 4;
        }
    }

    automata[0]['M'] = 97;
    automata[97]['O'] = 98;
    automata[98]['N'] = 99;
    automata[99]['I'] = 100;
    automata[100]['T'] = 101;
    automata[101]['O'] = 102;
    automata[102]['R'] = 103;
    automata[103]['I'] = 104;
    automata[104]['N'] = 105;
    automata[105]['G'] = 106;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'O')
        {
            automata[97][i] = 4;
        }
    }

    automata[0]['O'] = 107;
    automata[107]['R'] = 108;
    automata[108]['E'] = 109;

    automata[109]['B'] = 110;
    automata[110]['E'] = 111;
    automata[111]['D'] = 112;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'R')
        {
            automata[107][i] = 4;
        }
    }

    automata[0]['P'] = 113;

    automata[113]['A'] = 114;
    automata[114]['T'] = 115;
    automata[115]['H'] = 116;

    automata[113]['L'] = 117;
    automata[117]['A'] = 118;
    automata[118]['T'] = 119;
    automata[119]['F'] = 120;
    automata[120]['O'] = 121;
    automata[121]['R'] = 122;
    automata[122]['M'] = 123;

    automata[113]['O'] = 124;

    automata[124]['L'] = 125;
    automata[125]['Y'] = 126;

    automata[126]['G'] = 127;
    automata[127]['O'] = 128;
    automata[128]['N'] = 129;

    automata[126]['L'] = 130;
    automata[130]['I'] = 131;
    automata[131]['N'] = 132;
    automata[132]['E'] = 133;

    automata[124]['I'] = 134;
    automata[134]['N'] = 135;
    automata[135]['T'] = 136;

    automata[113]['U'] = 137;
    automata[137]['M'] = 138;
    automata[138]['P'] = 139;

    automata[113]['R'] = 140;
    automata[140]['O'] = 141;
    automata[141]['C'] = 142;
    automata[142]['E'] = 143;
    automata[143]['S'] = 144;
    automata[144]['S'] = 145;
    automata[145]['I'] = 146;
    automata[146]['N'] = 147;
    automata[147]['G'] = 148;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'R' || i != 'U' || i != 'O' || i != 'A' || i != 'L')
        {
            automata[113][i] = 4;
        }
    }

    automata[0]['R'] = 149;
    automata[149]['I'] = 150;
    automata[150]['V'] = 151;
    automata[151]['E'] = 152;
    automata[152]['R'] = 153;

    automata[150]['S'] = 154;
    automata[154]['E'] = 155;
    automata[155]['R'] = 156;

    automata[149]['E'] = 157;
    automata[157]['C'] = 158;
    automata[158]['O'] = 159;
    automata[159]['V'] = 160;
    automata[160]['E'] = 161;
    automata[161]['R'] = 162;
    automata[162]['Y'] = 163;

    automata[157]['T'] = 164;
    automata[164]['U'] = 165;
    automata[165]['R'] = 166;
    automata[166]['N'] = 167;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'I' || i != 'E')
        {
            automata[149][i] = 4;
        }
    }

    automata[0]['S'] = 168;
    automata[168]['T'] = 169;
    automata[169]['O'] = 170;
    automata[170]['R'] = 171;
    automata[171]['A'] = 172;
    automata[172]['G'] = 173;
    automata[173]['E'] = 174;

    automata[169]['R'] = 175;
    automata[175]['I'] = 176;
    automata[176]['P'] = 177;

    automata[168]['U'] = 178;
    automata[178]['R'] = 179;
    automata[179]['F'] = 180;
    automata[180]['A'] = 181;
    automata[181]['C'] = 182;
    automata[182]['E'] = 183;

    automata[168]['H'] = 184;
    automata[184]['A'] = 185;
    automata[185]['F'] = 186;
    automata[186]['T'] = 187;

    automata[168]['P'] = 188;
    automata[188]['O'] = 189;
    automata[189]['I'] = 190;
    automata[190]['L'] = 191;
    automata[191]['P'] = 192;
    automata[192]['I'] = 193;
    automata[193]['T'] = 194;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'T' || i != 'U' || i != 'H' || i != 'P')
        {
            automata[168][i] = 4;
        }
    }

    automata[0]['T'] = 195;
    automata[195]['U'] = 196;
    automata[196]['N'] = 197;
    automata[197]['N'] = 198;
    automata[198]['E'] = 199;
    automata[199]['L'] = 200;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'U')
        {
            automata[195][i] = 4;
        }
    }

    automata[0]['U'] = 201;
    automata[201]['N'] = 202;
    automata[202]['D'] = 203;
    automata[203]['E'] = 204;
    automata[204]['R'] = 205;
    automata[205]['G'] = 206;
    automata[206]['R'] = 207;
    automata[207]['O'] = 208;
    automata[208]['U'] = 209;
    automata[209]['N'] = 210;
    automata[210]['D'] = 211;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'N')
        {
            automata[201][i] = 4;
        }
    }

    automata[0]['W'] = 212;
    automata[212]['A'] = 213;
    automata[213]['S'] = 214;
    automata[214]['H'] = 215;
    automata[215]['I'] = 216;
    automata[216]['N'] = 217;
    automata[217]['G'] = 218;

    automata[212]['E'] = 219;
    automata[219]['L'] = 220;
    automata[220]['L'] = 221;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        if (i != 'A' || i != 'E')
        {
            automata[212][i] = 4;
        }
    }

    automata[0]['#'] = automata[2]['#'] = 2;

    for (int i = 'A'; i <= 'Z'; i++)
    {
        automata[0][i] = automata[4][i] = 4;
    }
    for (int i = 'a'; i <= 'z'; i++)
    {
        automata[0][i] = automata[4][i] = 4;
    }

    for (int i = '0'; i <= '9'; i++)
    {
        automata[0][i] = automata[1][i] = 1;

        automata[2][i] = automata[3][i] = 3;

        automata[4][i] = automata[5][i] = 5;
    }

    for (int i = 'a'; i <= 'f'; i++)
    {
        automata[2][i] = automata[3][i] = 3;
    }

    for (int i = 'A'; i <= 'F'; i++)
    {
        automata[2][i] = automata[3][i] = 3;
    }

    automata[0]['\\'] = 6;
    automata[6]['+'] = automata[7]['+'] = 7;
    automata[6]['*'] = automata[9]['*'] = 9;

    automata[0]['-'] = 8;
    automata[0]['/'] = 10;
    automata[0]['&'] = 11;
    automata[0]['|'] = 12;
    automata[0]['('] = 13;
    automata[0][')'] = 14;

    automata[0][';'] = 15;
    automata[0][':'] = 16;
    automata[16]['='] = automata[17]['='] = 17;
    automata[0]['*'] = 18;

    automata[0][' '] = 37;
    automata[0]['\n'] = 38;
    automata[0]['\t'] = 39;
    automata[0]['\r'] = 40;

    automata[0]['+'] = 41;

    automata[0][','] = 222;
    automata[0]['{'] = 223;
    automata[0]['}'] = 224;
    automata[0]['['] = 225;
    automata[0][']'] = 226;

    finite[0] = tLexError;
    finite[1] = tInteger;

    finite[2] = tLexError;

    finite[3] = tHex;

    finite[4] = tVarLetter;
    finite[5] = tVarDigit;
    finite[6] = tLexError;

    finite[7] = tPlus;
    finite[9] = tTimes;

    finite[8] = tMinus;
    finite[10] = tSlash;
    finite[11] = tAND;
    finite[12] = tOR;
    finite[13] = tBracketOpen;
    finite[14] = tBracketClosed;

    finite[15] = tSemiColon;
    finite[16] = tIgnore;
    finite[17] = tEquals;
    finite[18] = tOnlyTimes;
    finite[19] = tIgnore;
    finite[20] = tIgnore;
    finite[21] = tFor;
    finite[22] = tIgnore;
    finite[23] = tIgnore;
    finite[24] = tIgnore;
    finite[25] = tIgnore;
    finite[26] = tBegin;
    finite[27] = tIgnore;
    finite[28] = tIgnore;
    finite[29] = tEnd;
    finite[30] = tIgnore;
    finite[31] = tIgnore;
    finite[32] = tIgnore;
    finite[33] = tIgnore;
    finite[34] = tIgnore;
    finite[35] = tIgnore;
    finite[36] = tConsole;

    finite[37] = tIgnore;
    finite[38] = tIgnore;
    finite[39] = tIgnore;
    finite[40] = tIgnore;
    finite[41] = tOnlyPlus;

    finite[42] = tIgnore;
    finite[43] = tBend;
    finite[44] = tIgnore;
    finite[45] = tBench;
    finite[46] = tIgnore;
    finite[47] = tBox;

    finite[48] = tIgnore;
    finite[49] = tIgnore;
    finite[50] = tCall;
    finite[51] = tIgnore;
    finite[52] = tIgnore;
    finite[53] = tIgnore;
    finite[54] = tIgnore;
    finite[55] = tCircle;

    finite[56] = tIgnore;
    finite[57] = tIgnore;
    finite[58] = tIgnore;
    finite[59] = tIgnore;
    finite[60] = tIgnore;
    finite[61] = tIgnore;
    finite[62] = tElevator;

    finite[63] = tIgnore;
    finite[64] = tIgnore;
    finite[65] = tIgnore;
    finite[66] = tIgnore;
    finite[67] = tIgnore;
    finite[68] = tIgnore;
    finite[69] = tDeepsea;

    finite[70] = tIgnore;
    finite[71] = tIgnore;
    finite[72] = tIgnore;
    finite[73] = tIgnore;
    finite[74] = tHuman;

    finite[75] = tIgnore;
    finite[76] = tIgnore;
    finite[77] = tIgnore;
    finite[78] = tIgnore;
    finite[79] = tIgnore;
    finite[80] = tIgnore;
    finite[81] = tInplace;
    finite[82] = tIgnore;
    finite[83] = tIgnore;
    finite[84] = tIgnore;
    finite[85] = tInsitu;
    finite[86] = tIgnore;
    finite[87] = tIgnore;
    finite[88] = tIgnore;
    finite[89] = tIgnore;
    finite[90] = tIgnore;
    finite[91] = tIgnore;
    finite[92] = tInjection;

    finite[93] = tIgnore;
    finite[94] = tIgnore;
    finite[95] = tIgnore;
    finite[96] = tLine;

    finite[97] = tIgnore;
    finite[98] = tIgnore;
    finite[99] = tIgnore;
    finite[100] = tIgnore;
    finite[101] = tIgnore;
    finite[102] = tIgnore;
    finite[103] = tIgnore;
    finite[104] = tIgnore;
    finite[105] = tIgnore;
    finite[106] = tMonitoring;

    finite[107] = tIgnore;
    finite[108] = tIgnore;
    finite[109] = tOre;
    finite[110] = tIgnore;
    finite[111] = tIgnore;
    finite[112] = tOreBed;

    finite[113] = tIgnore;
    finite[114] = tIgnore;
    finite[115] = tIgnore;
    finite[116] = tPath;
    finite[117] = tIgnore;
    finite[118] = tIgnore;
    finite[119] = tIgnore;
    finite[120] = tIgnore;
    finite[121] = tIgnore;
    finite[122] = tIgnore;
    finite[123] = tPlatform;
    finite[124] = tIgnore;
    finite[125] = tIgnore;
    finite[126] = tIgnore;
    finite[127] = tIgnore;
    finite[128] = tIgnore;
    finite[129] = tPolygon;
    finite[130] = tIgnore;
    finite[131] = tIgnore;
    finite[132] = tIgnore;
    finite[133] = tPolyline;
    finite[134] = tIgnore;
    finite[135] = tIgnore;
    finite[136] = tPoint;
    finite[137] = tIgnore;
    finite[138] = tIgnore;
    finite[139] = tPump;
    finite[140] = tIgnore;
    finite[141] = tIgnore;
    finite[142] = tIgnore;
    finite[143] = tIgnore;
    finite[144] = tIgnore;
    finite[145] = tIgnore;
    finite[146] = tIgnore;
    finite[147] = tIgnore;
    finite[148] = tProcessing;

    finite[149] = tIgnore;
    finite[150] = tIgnore;
    finite[151] = tIgnore;
    finite[152] = tIgnore;
    finite[153] = tRiver;
    finite[154] = tIgnore;
    finite[155] = tIgnore;
    finite[156] = tRiser;
    finite[157] = tIgnore;
    finite[158] = tIgnore;
    finite[159] = tIgnore;
    finite[160] = tIgnore;
    finite[161] = tIgnore;
    finite[162] = tIgnore;
    finite[163] = tRecovery;
    finite[164] = tIgnore;
    finite[165] = tIgnore;
    finite[166] = tIgnore;
    finite[167] = tReturn;

    finite[168] = tIgnore;
    finite[169] = tIgnore;
    finite[170] = tIgnore;
    finite[171] = tIgnore;
    finite[172] = tIgnore;
    finite[173] = tIgnore;
    finite[174] = tStorage;
    finite[175] = tIgnore;
    finite[176] = tIgnore;
    finite[177] = tStrip;
    finite[178] = tIgnore;
    finite[179] = tIgnore;
    finite[180] = tIgnore;
    finite[181] = tIgnore;
    finite[182] = tIgnore;
    finite[183] = tSurface;
    finite[184] = tIgnore;
    finite[185] = tIgnore;
    finite[186] = tIgnore;
    finite[187] = tShaft;
    finite[188] = tIgnore;
    finite[189] = tIgnore;
    finite[190] = tIgnore;
    finite[191] = tIgnore;
    finite[192] = tIgnore;
    finite[193] = tIgnore;
    finite[194] = tSpoilpit;

    finite[195] = tIgnore;
    finite[196] = tIgnore;
    finite[197] = tIgnore;
    finite[198] = tIgnore;
    finite[199] = tIgnore;
    finite[200] = tTunnel;
    finite[201] = tIgnore;
    finite[202] = tIgnore;
    finite[203] = tIgnore;
    finite[204] = tIgnore;
    finite[205] = tIgnore;
    finite[206] = tIgnore;
    finite[207] = tIgnore;
    finite[208] = tIgnore;
    finite[209] = tIgnore;
    finite[210] = tIgnore;
    finite[211] = tUnderground;

    finite[212] = tIgnore;
    finite[213] = tIgnore;
    finite[214] = tIgnore;
    finite[215] = tIgnore;
    finite[216] = tIgnore;
    finite[217] = tIgnore;
    finite[218] = tWashing;
    finite[219] = tIgnore;
    finite[220] = tIgnore;
    finite[221] = tWell;

    finite[222] = tComma;
    finite[223] = tSquareBracketOpen;
    finite[224] = tSquareBracketClosed;
    finite[225] = tCurlyBracketOpen;
    finite[226] = tCurlyBracketClosed;
}

//Metoda ki vrne naslednjo stanje
int Lexer::getNextState(int aState, int aChar) const
{
    if (aChar == -1)
    {
        return noEdge;
    }

    return automata[aState][aChar];
}

//Metoda, ki preveri ali je stanje končno
bool Lexer::isFiniteState(int aState) const
{
    return finite[aState] != tLexError;
}

//Metoda, ki pridobi knčno stanje
int Lexer::getFiniteState(int aState) const
{
    return finite[aState];
}

//Vrne naslednji char
int Lexer::peek()
{
    return input->peek();
}

//Metoda, ki dobi char
int Lexer::read()
{
    int temp = input->get();
    col++;
    if (temp == '\n')
    {
        row++;
        col = 1;
    }
    return temp;
}

//Metoda, ki zajame token
Token Lexer::nextTokenImp()
{
    int currentState = startState;
    std::string lexem;
    int startCol = col;
    int startRow = row;

    do
    {
        int tempState = getNextState(currentState, peek());
        if (tempState != noEdge)
        {
            currentState = tempState;
            lexem += (char)read();
        }
        else
        {
            if (isFiniteState(currentState))
            {
                Token token(lexem, startCol, startRow, getFiniteState(currentState), eof());
                if (token.getToken() == tIgnore)
                {
                    return nextToken();
                }
                else
                {
                    return token;
                }
            }
            else
            {
                return Token("", startCol, startRow, tLexError, eof());
            }
        }
    }

    while (true);
}

//Metoda preveri ali je konec fila
bool Lexer::eof()
{
    return input->peek() == -1;
}

Lexer::Lexer(std::istream *aInput)
{
    row = 1;
    col = 1;
    initAutomata();
    input = aInput;
}

//Metoda ki vrne naslednji token
Token Lexer::nextToken()
{
    return lastToken = nextTokenImp();
}

//Metoda ki vrne trenutni token
Token Lexer::currentToken()
{
    return lastToken;
}

//Metoda, ki izpiše vse treminale (za test)
void Lexer::printAllTerminals()
{
    while (!eof())
    {
        Token token = nextToken();
        if (token.getToken() == tLexError)
        {
            std::cerr << "Napaka vrstica (" << token.getRow() << ") stolpec (" << token.getCol() << ")\n";
            break;
        }
        std::cout << "Terminal(" << token.getLexem() << ")\n";
    }
}

//Metoda, ki vstavi variable v dictionary
void Lexer::insertVariable(const std::string &name, double value)
{
    this->dictionary[name] = value;
}

//Metoda, ki pridobi variable iz dictionary
double Lexer::getVarValue(std::string &name)
{
    if (this->dictionary.find(name) != this->dictionary.end())
    {
        return this->dictionary[name];
    }
    else
    {
        return -1;
        std::cerr << "Var ne obstaja\n";
    }
}

//Metoda, ki posodib variable v dictionary
void Lexer::updateVarValue(std::string &name, double value)
{
    this->dictionary.at(name) = value;
}

//Metoda, ki pridobi trenutno pozicijo 
std::streampos Lexer::getPosition()
{
    return input->tellg();
}

//Metoda, ki zamenja pozicijo
void Lexer::changePosition(std::streampos &pos)
{
    input->seekg(pos);
    input->clear();
}