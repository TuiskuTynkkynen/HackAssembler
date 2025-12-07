#pragma once

#include "Instructions.h"
#include "SymbolMap.h"

#include <vector>
#include <optional>

namespace Parser {
    class ParseResult {
    public:
        std::vector<std::variant<AddressingInstruction, ComputeInstruction>> Instructions;
        SymbolMap Symbols;

        static std::vector<std::variant<LoadInstruction, ComputeInstruction>> ResolveSymbols(ParseResult& parsed);
    };

    std::optional<ParseResult> Parse(std::istream& stream);
};