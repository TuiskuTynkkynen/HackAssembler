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
    return character == '=' || character == ';' || character == '+' || character == '-' || character == '&' || character == '|';
}

static constexpr bool IsNumeric(char character) {
    return '0' <= character && character <= '9';
}

static constexpr bool IsAlphaNumeric(char character) {
    return IsNumeric(character) || 'A' <= character && character <= 'Z' || 'a' <= character && character <= 'z';
}

Lexer::Token Lexer::GetNextToken(std::istream& stream) {
    std::string token;
    char character;
    State state = State::None;
    TokenType tokenType = TokenType::EndOfStream;
    
    while (state != State::Terminal && stream.get(character).good()) {
        tokenType = TokenType::Value(TokenType::Invalid + (uint8_t)state);

        switch (state) {
        case State::None:
            if (character == '/' && stream.peek() == '/') { state = State::Comment; break; }
            if (IsOperation(character)) { state = State::Operation; break; }
            if (character == '(') { stream.get(character); state = State::Label; break; }
            if (IsNumeric(character)) { state = State::Integer; break; }
            if (IsAlphaNumeric(character)) { state = State::String; break; }
            break;
        case State::Comment:
            state = (character == '\n') ? State::Terminal : state;
            break;
        case State::Operation:
            state = State::Terminal;
            break;
        case State::Label:
            if (!IsAlphaNumeric(character)) {
                state = State::Terminal;
                stream.get();
            }
            break;
        case State::Integer:
            state = !IsNumeric(character) ? State::Terminal : state;
            break;
        case State::String:
            tokenType = (character == ':') ? TokenType::Label : TokenType::String;
            state = !IsAlphaNumeric(character) ? State::Terminal : state;
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