#include "Lexer.h"

#include <iostream>
#include <sstream>

int main() {
    std::stringstream stream("@35         // A_register holds 35\nD = A       // D_register holds 35\n@100        // A_register holds 100\nD = D + A   // D_register holds 135\n");
    
    Lexer::Token token;
    while (token.Type != Lexer::TokenType::EndOfStream) {
        token = Lexer::GetNextToken(stream);

        std::cout << token.Data << "\t: " << token.Type.ToString() << "\n";
    }
}