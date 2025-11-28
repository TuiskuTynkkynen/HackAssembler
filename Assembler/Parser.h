#pragma once

#include "Instructions.h"

#include <vector>
#include <optional>

namespace Parser {
    struct ParseResult {
        std::vector<std::variant<AddressingInstruction, ComputeInstruction>> Instructions;
    };

    std::optional<ParseResult> Parse(std::istream& stream);
};