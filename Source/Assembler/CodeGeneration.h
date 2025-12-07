#pragma once

#include "Parser.h"

#include <vector>

namespace CodeGeneration {
    std::vector<uint16_t> GenerateCode(const std::vector<std::variant<LoadInstruction, ComputeInstruction>>& instructions);
};