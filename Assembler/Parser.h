#pragma once

#include "Instructions.h"

#include <vector>
#include <optional>

namespace Parser {
    struct ParseResult {
        std::vector<Instruction> Instructions;
    };

    std::optional<ParseResult> Parse(std::istream& stream);
};