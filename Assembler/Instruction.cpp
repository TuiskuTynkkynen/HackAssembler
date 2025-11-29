#include "Instructions.h"

#include <format>

static constexpr bool IsUnaryOperation(const SemanticToken& semantic) {
    auto ptr = semantic.TryGetData<Operations>();
    return ptr && (*ptr == Operations::Negation || *ptr == Operations::Substraction);
}

static constexpr std::expected<Operands, Instructions::ParseError> TryParseOperand(const SemanticToken& semantic, uint8_t stackIndex) {
    if (const uint16_t* integer = semantic.TryGetData<uint16_t>(); integer) {
        if (*integer > 1) {
            return std::unexpected<Instructions::ParseError>(std::in_place, Instructions::ParseError::InvalidOperand, stackIndex);
        }

        return *integer == 1 ? Operands::One : Operands::Zero;
    }

    if (const ::Destination* registers = semantic.TryGetData<::Destination>(); registers) {
        if (registers->A + registers->D + registers->M != 1) {
            return std::unexpected<Instructions::ParseError>(std::in_place, Instructions::ParseError::InvalidOperand, stackIndex);
        }

        if (registers->A) { return Operands::A; }
        if (registers->D) { return Operands::D; }
        if (registers->M) { return Operands::M; }
    }

    return std::unexpected<Instructions::ParseError>(std::in_place, Instructions::ParseError::InvalidOperand, stackIndex);
}

std::expected<ComputeInstruction, Instructions::ParseError> ComputeInstruction::Create(std::span<const SemanticToken> semanticStack) {
    if (1 >= semanticStack.size() || semanticStack.size() > 7){
        return std::unexpected<Instructions::ParseError>(std::in_place, Instructions::ParseError::InvalidSemanticTokenCount, -1);
    }

    ComputeInstruction result = {};

    const int32_t maxIndex = static_cast<int32_t>(semanticStack.size()) - 1;
    for (int32_t currentIndex = maxIndex; 0 < currentIndex; currentIndex -= 2) {
        if (currentIndex == 1 && semanticStack[0].GetType() == SemanticToken::Type::Registers) { // Ugly hack
            break; 
        }
        
        const Operations* operation = semanticStack[currentIndex - 1].TryGetData<Operations>();
        if (!operation) {
            return std::unexpected<Instructions::ParseError>(std::in_place, Instructions::ParseError::InvalidOperationOrder, currentIndex - 1);
        }

        if (*operation == Operations::Jump) {
            if (currentIndex != maxIndex) {
                return std::unexpected<Instructions::ParseError>(std::in_place, Instructions::ParseError::InvalidOperationOrder, currentIndex - 1);
            }

            auto jump = semanticStack[currentIndex].TryGetData<Jumps>();
            if (!jump) {
                return std::unexpected<Instructions::ParseError>(std::in_place, Instructions::ParseError::InvalidJump, currentIndex);
            }

            result.Jump = *jump;
            continue;
        }

        if (*operation == Operations::Assignment) {
            if (currentIndex != 2) { // Must be last operation
                return std::unexpected<Instructions::ParseError>(std::in_place, Instructions::ParseError::InvalidOperationOrder, currentIndex - 1);
            }

            auto res = IsUnaryOperation(semanticStack[currentIndex]) ? Operands::None : TryParseOperand(semanticStack[currentIndex], static_cast<uint8_t>(currentIndex));
            if (!res.has_value()) {
                return std::unexpected(res.error());
            }

            result.Comparison.Left = res.value();
            break;
        }

        auto res = TryParseOperand(semanticStack[currentIndex], static_cast<uint8_t>(currentIndex));
        if (!res.has_value()) {
            return std::unexpected(res.error());
        }

        result.Comparison.Right = res.value();
        result.Comparison.Operation = *operation;
    }

    if (semanticStack[1].HasValue(Operations::Assignment)) {
        auto destination = semanticStack[0].TryGetData<::Destination>();
        if (!destination) {
            return std::unexpected<Instructions::ParseError>(std::in_place, Instructions::ParseError::InvalidDestination, 0);
        }

        result.Destination = *destination;
    }

    if (semanticStack[1].HasValue(Operations::Jump)) {
        auto res = TryParseOperand(semanticStack[0], 0);
        if (!res.has_value()) {
            return std::unexpected(res.error());
        }
    
        result.Comparison.Right = res.value();
    }

    return result;
}

std::string ComputeInstruction::ToString() const {
    auto destinationToString = [](::Destination dest) {
        std::string_view A = dest.A ? "A" : "";
        std::string_view D = dest.D ? "D" : "";
        std::string_view M = dest.M ? "M" : "";
        
        return std::format("{}{}{}", A, D, M);
        };

    auto operandToString = [](Operands o) {
        switch (o) {
        case Operands::Zero:
            return " 0";
        case Operands::One:
            return " 1";
        case Operands::A:
            return " A";
        case Operands::D:
            return " D";
        case Operands::M:
            return " M";
        }

        return "";
        };

    auto operationToString = [](Operations op) {
        switch (op) {
        case Operations::Assignment:
            return " =";
        case Operations::Negation:
            return " !";
        case Operations::Addition:
            return " +";
        case Operations::Substraction:
            return " -";
        case Operations::BitwiseAnd:
            return " &";
        case Operations::BitwiseOr:
            return " |";
        case Operations::Jump:
            return " ;";
        }

        return "";
        };

    auto jumpToString = [](Jumps jump) {
        switch (jump) {
        case Jumps::JMP:
            return " ; JMP";
        case Jumps::JGT:
            return " ; JGT";
        case Jumps::JEQ:
            return " ; JEQ";
        case Jumps::JLT:
            return " ; JLT";
        case Jumps::JGE:
            return " ; JGE";
        case Jumps::JLE:
            return " ; JLE";
        case Jumps::JNE:
            return " ; JNE";
        }

        return "";
        };

    bool destination = Destination.A || Destination.D || Destination.M;
    bool comparison = Comparison.Left != Operands::None || Comparison.Right != Operands::None;
    std::string_view eq = destination && comparison ? " =" : "";
    return std::format("{}{}{}{}{}{}", destinationToString(Destination), eq, operandToString(Comparison.Left), operationToString(Comparison.Operation), operandToString(Comparison.Right), jumpToString(Jump));
}

std::expected<AddressingInstruction, Instructions::ParseError> AddressingInstruction::Create(const SemanticToken& semantic) {
    if (const uint16_t* integer = semantic.TryGetData<uint16_t>(); integer) {
        return AddressingInstruction{ *integer };
    }

    if (const std::string* variable = semantic.TryGetData<std::string>(); variable) {
        return AddressingInstruction{ *variable };
    }

    return std::unexpected<Instructions::ParseError>(std::in_place, Instructions::ParseError::InvalidSemanticTokenType, 0);
}

std::string AddressingInstruction::ToString() const {
    return std::visit([](const auto& obj) { return std::format("Load {}", obj); }, Variable);
}

std::string LoadInstruction::ToString() const {
    return std::format("Load {}", value);
}