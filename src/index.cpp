#include <iostream>
#include "Lexer.hpp"
#include "Token.hpp"

int main(int argc, char *argv[])
{
    std::ifstream file("test/test.txt");
    Lexer lexer(&file);

    lexer.printAllTerminals();
    return 0;
}