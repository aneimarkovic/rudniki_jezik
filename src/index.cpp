#include <iostream>
#include "Lexer.hpp"
#include "Token.hpp"
#include "Parser.hpp"

int main(int argc, char *argv[])
{
    std::ifstream file("./test/ParserTest.txt");
    Lexer lexer(&file);

    Parser* parser = new Parser(lexer);

    if (parser->parseFile("./test/ParserTest.txt"))
    {
        std::cout << "Accept";
    }
    else
    {
        std::cout << "Reject";
    }

    std::cin.get();

    file.close();
    delete parser;
    return 0;
}