#pragma once

#include <string>
#include <string_view>
#include <cassert>

namespace Lexer {
    class TokenType {
    public:
        enum Value : uint8_t {
            Invalid = 0,
            Comment,
            Operation,
            Label,
            Integer,
            String,
            EndOfStream,
        };

        constexpr TokenType() = default;

        constexpr TokenType(Value type) {
            m_Type = type;
        }

        constexpr void operator=(Value newType) {
            m_Type = newType;
        }

        constexpr operator Value() const {
            return m_Type;
        }

        inline constexpr std::string_view ToString() const {
            switch (m_Type) {
            case Invalid:   return "Invalid";
            case Comment:   return "Comment";
            case Operation: return "Operation";
            case Label:     return "Label";
            case Integer:   return "Integer";
            case String:  return "String";
            case EndOfStream:  return "EndOfStream";
            }

            assert(false);
            return "Invalid";
        }
    private:
        Value m_Type = Invalid;
    };

    struct Token {
        TokenType Type;
        std::string Data;
    };

    Token GetNextToken(std::istream& stream);
};

