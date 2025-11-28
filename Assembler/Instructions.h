#pragma once

#include "SemanticToken.h"

#include <string>
#include <span>
#include <variant>

enum class Operands : uint8_t {
    None = 0,
    Zero,
    One,
    A,
    D,
    M,
};

struct Comparison {
    Operands Left;
    Operands Right;
    Operations Operation;
};

namespace Instructions {
    struct ParseError {
        enum Types : uint8_t {
            InvalidSemanticTokenType,
            InvalidSemanticTokenCount,
            InvalidOperationOrder,
            InvalidDestination,
            InvalidOperand,
            InvalidJump,
        };

        Types Type;
        uint8_t StackIndex;

        ParseError(Types type, uint8_t stackIndex) : Type(type), StackIndex(stackIndex) {}
    };
}

struct ComputeInstruction {
    Destination Destination;
    Comparison Comparison;
    Jumps Jump;


    static std::expected<ComputeInstruction, Instructions::ParseError> Create(std::span<const SemanticToken> semanticTokens);
    std::string ToString() const;
};

struct AddressingInstruction {
    std::variant<std::string, uint16_t> Variable;

    static std::expected<AddressingInstruction, Instructions::ParseError> Create(const SemanticToken& semanticToken);
    std::string ToString() const;
};