#pragma once

#include "Lexer.h"

#include <string>
#include <variant>
#include <expected>

enum class Operations : uint8_t {
    None = 0,
    Assignment,
    Negation,
    Addition,
    Subtraction,
    BitwiseAnd,
    BitwiseOr,
    Jump,
};

enum class Jumps : uint8_t {
    None = 0,
    JMP,
    JGT,
    JEQ,
    JLT,
    JGE,
    JLE,
    JNE,
};

struct Destination {
    bool A : 1 = false;
    bool D : 1 = false;
    bool M : 1 = false;
};

class SemanticToken {
public:
    enum class Type : uint8_t {
        Registers,
        Integer,
        Variable,
        Jump,
        Operation,
    };

    enum class ParseError {
        Unexpected = 0,
        InvalidTokenType,
        InvalidTokenData,
        IntegerOutOfRange,
    };
    static std::expected<SemanticToken, ParseError> Create(const Lexer::Token& token);

    bool ValidAfter(const SemanticToken& previous) const;
    inline constexpr Type GetType() const { return static_cast<Type>(m_Data.index()); };
    
    template <typename T>
    inline constexpr const T& GetData() const { return std::get<T>(m_Data); };
    template <typename T>
    inline constexpr const T* TryGetData() const { return std::get_if<T>(&m_Data); };
    template <typename T>
    inline constexpr bool HasValue(const T& value) const {
        const T* ptr = TryGetData<T>();
        return ptr && *ptr == value;
    }
private:
    SemanticToken(auto data) : m_Data(data) {}
    std::variant<Destination, uint16_t, std::string, Jumps, Operations> m_Data;
};

