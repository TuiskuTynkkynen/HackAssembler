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

struct ComputeInstruction {
    Destination Destination;
    Comparison Comparison;
    Jumps Jump;

    enum class ParseError : uint8_t {
        InvalidSemanticTokenCount,
        InvalidOperationOrder,
        InvalidDestination,
        InvalidOperand,
        InvalidJump,
    };

    static std::expected<ComputeInstruction, ParseError> Create(std::span<const SemanticToken> semanticTokens);
    std::string ToString() const;
};

struct AddressingInstruction {
    std::variant<std::string, uint16_t> Variable;

    enum class ParseError : uint8_t {
        InvalidSemanticTokenType,
    };

    static std::expected<AddressingInstruction, ParseError> Create(const SemanticToken& semanticToken);
    std::string ToString() const;
};

using Instruction = std::variant<AddressingInstruction, ComputeInstruction>;