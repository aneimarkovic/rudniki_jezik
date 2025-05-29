#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.hpp"
#include "Parser.hpp"

int main(int argc, char *argv[]) {
    std::ifstream file("./test/ParserTest.txt");
    Lexer lexer(&file);

    Parser* parser = new Parser(lexer);

    if (parser->parseFile("./test/ParserTest.txt")) {
        std::cout << "Accept\n";
        std::cout << "\n--- AST Tree ---" << std::endl;
        
        std::unique_ptr<ProgramNode> astRoot = parser->getProgramNode();
        if (astRoot) {
            astRoot->printGeoJSON(std::cout, 0);
        } else {
            std::cout << "AST Root is null even after successful parse." << std::endl;
        }
    } else {
        std::cout << "Reject\n";
    }

    std::cin.get();

    return 0;
}