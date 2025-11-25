#include "Lexer.h"

#include <istream>

enum class State : uint8_t {
    None = 0,
    Comment,
    Operation,
    Label,
    Integer,
    String,
    Terminal
};

static constexpr bool IsOperation(char character) {
    return character == '=' || character == ';' || character == '!' || character == '+' || character == '-' || character == '&' || character == '|';
}

static constexpr bool IsNumeric(char character) {
    return '0' <= character && character <= '9';
}

static constexpr bool IsAlpha(char character) {
    character &= ~32;
    return 'A' <= character && character <= 'Z';
}

static constexpr bool IsSpecial(char character) {
    return  character == '_' || character == '.' || character == '$' || character == ':';
}

static constexpr bool IsSymbol(char character) {
    return IsNumeric(character) || IsAlpha(character) || IsSpecial(character);
}

Lexer::Token Lexer::GetNextToken(std::istream& stream) {
    std::string token;
    char character;
    State state = State::None;
    TokenType tokenType = TokenType::EndOfStream;
    
    while (state != State::Terminal && stream.get(character).good()) {
        switch (state) {
        case State::None:
            if (character == '/' && stream.peek() == '/') { state = State::Comment; }
            if (IsOperation(character)) { state = State::Operation; }
            if (character == '(') { stream.get(character); state = State::Label; }
            if (IsNumeric(character)) { state = State::Integer; }
            if (IsAlpha(character) || IsSpecial(character)) { state = State::String; }

            tokenType = TokenType::Value(TokenType::Invalid + (uint8_t)state);
            break;
        case State::Comment:
            state = (character == '\n') ? State::Terminal : state;
            break;
        case State::Operation:
            state = State::Terminal;
            break;
        case State::Label:
            if (!IsSymbol(character)) {
                state = State::Terminal;
                stream.get();
            }
            break;
        case State::Integer:
            state = !IsNumeric(character) ? State::Terminal : state;
            break;
        case State::String:
            state = !IsSymbol(character) ? State::Terminal : state;
            break;
        }

        if (state == State::Terminal) {
            stream.unget();
            break;
        }

        if (state != State::None && state != State::Terminal) {
            token.append(1, character);
        }
    }

    return { tokenType, token};
}