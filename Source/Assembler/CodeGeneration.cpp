#include "CodeGeneration.h"

#include <bitset>

static constexpr uint16_t ToCode(Jumps jump) {
    return static_cast<uint16_t>(jump);
}

static constexpr uint16_t ToCode(Destination destination) {
    uint16_t res = 0b100 * destination.A 
                 | 0b010 * destination.D 
                 | 0b001 * destination.M;
    return res;
}

static constexpr uint16_t ToCode(Comparison comparison) {
    bool hasAM = comparison.Left == Operands::A || comparison.Right == Operands::A
                || comparison.Left == Operands::M || comparison.Right == Operands::M;
    bool hasD = comparison.Left == Operands::D || comparison.Right == Operands::D;
    bool subtract = comparison.Operation == Operations::Subtraction;
    bool add = comparison.Operation == Operations::Addition;

    if (!hasAM && !hasD) {
        if (subtract) { // -1 ->  0b0111010 
            return 0b0111010;
        }

        //  1 ->  0b0111111, 0 ->  0b0101010 
        return comparison.Left == Operands::One ? 0b0111111 : 0b0101010;
    }
    
    std::bitset<7> result;
    // MSB of comparison field swaps between A and M register
    result[6] = comparison.Left == Operands::M || comparison.Right == Operands::M;

    if (hasAM && hasD) {
        if (subtract) { // D-AM -> 0bx010011, AM-D -> 0bx000111
            result |= (comparison.Right == Operands::D) ? 0b000111 : 0b010011;
        }

        result |= 0b000010 * add; // D+AM -> 0bx000010

        // D|AM -> 0bx10101, D&AM -> 0bx00000
        result |= 0b010101 * (comparison.Operation == Operations::BitwiseOr);

        return static_cast<uint16_t>(result.to_ulong());
    }

    /*
    D     0bx001100
    AM    0bx110000
    !D    0bx001101
    !AM   0bx110001
    -D    0bx001111
    -AM   0bx110011
    D+1   0bx011111
    AM+1  0bx110111
    D-1   0bx001110
    AM-1  0bx110010
    */
    result |= 0b001100 * hasD;
    result |= 0b110000 * hasAM;

    result |= 0b000001 * (comparison.Operation == Operations::Negation);
    result |= 0b000001 * (comparison.Right != Operands::One && subtract);
    
    result |= 0b010111 * add;
    result |= 0b000010 * (subtract);

    return static_cast<uint16_t>(result.to_ulong());
}

static constexpr uint16_t Code(const ComputeInstruction& instruction){
    //Identifier: 3bits  Comparison: 7 bits                    Destination: 3 bits                   Jump: 3 bits 
    return 0b111 << 13 | ToCode(instruction.Comparison) << 6 | ToCode(instruction.Destination) << 3 | ToCode(instruction.Jump) << 0;
}

static constexpr uint16_t Code(const LoadInstruction& instruction) {
    return instruction.value;
}

std::vector<uint16_t> CodeGeneration::GenerateCode(const std::vector<std::variant<LoadInstruction, ComputeInstruction>>& instructions) {
    std::vector<uint16_t> result;
    result.reserve(instructions.size());

    for (size_t i = 0; i < instructions.size(); i++) {
        result.emplace_back(std::visit([](const auto& v) { return Code(v); }, instructions[i]));
    }

    return result;
}