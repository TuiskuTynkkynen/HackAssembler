#include "SemanticToken.h"

#include <optional>
#include <charconv>
#include <limits>

static constexpr std::expected<Operations, SemanticToken::ParseError> ParseOperation(const std::string_view token) {
    if (token.size() != 1) {
        return std::unexpected(SemanticToken::ParseError::InvalidTokenData);
    }

    switch (token.front()) {
    case '=':
        return Operations::Assignment;
    case '!':
        return Operations::Negation;
    case '+':
        return Operations::Addition;
    case '-':
        return Operations::Subtraction;
    case '&':
        return Operations::BitwiseAnd;
    case '|':
        return Operations::BitwiseOr;
    case ';':
        return Operations::Jump;
    }

    assert(false); // Should never reach this
    return std::unexpected(SemanticToken::ParseError::InvalidTokenData);
}

static constexpr std::expected<uint16_t, SemanticToken::ParseError> ParseInteger(const std::string_view token) {
    int32_t number = 0;
    std::from_chars_result result = std::from_chars(token.data(), token.data() + token.size(), number);

    if (result.ptr != token.data() + token.size()) {
        return std::unexpected(SemanticToken::ParseError::InvalidTokenData);
    }

    if (result.ec == std::errc::result_out_of_range || number < 0 || number > std::numeric_limits<uint16_t>::max()) {
        return std::unexpected(SemanticToken::ParseError::IntegerOutOfRange);
    }

    return static_cast<uint16_t>(number);
}

static constexpr std::optional<Destination> TryParseRegisters(const std::string_view token) {
    size_t registerCount = 0;
    Destination registers{};

    constexpr auto ContainsAt = [](const std::string_view str, char character, size_t index) {
        return str.length() > index && str[index] == character;
        };

    registerCount += registers.A = ContainsAt(token, 'A', registerCount);

    registerCount += registers.D = ContainsAt(token, 'D', registerCount);

    registerCount += registers.M = ContainsAt(token, 'M', registerCount);

    if (token.length() == registerCount) {
        return registers;
    }

    return std::nullopt;
}

static constexpr std::optional<Jumps> TryParseJump(const std::string_view token) {
    if (token == "JMP") return Jumps::JMP;
    if (token == "JGT") return Jumps::JGT;
    if (token == "JEQ") return Jumps::JEQ;
    if (token == "JLT") return Jumps::JLT;
    if (token == "JGE") return Jumps::JGE;
    if (token == "JLE") return Jumps::JLE;
    if (token == "JNE") return Jumps::JNE;

    return std::nullopt;
}

static constexpr std::expected<std::string, SemanticToken::ParseError> ParseVariable(const std::string_view token) {
    if (token.empty()) {
        return std::unexpected(SemanticToken::ParseError::InvalidTokenData);
    }

    return std::string(token);
}

std::expected<SemanticToken, SemanticToken::ParseError> SemanticToken::Create(const Lexer::Token& token) {
    constexpr auto construct = [](auto data) {return SemanticToken(data); };

    std::optional<Destination> registers;
    std::optional<Jumps> jump;

    switch (token.Type) {
    case Lexer::TokenType::Operation:
        return ParseOperation(token.Data).transform(construct);
    case Lexer::TokenType::Integer:
        return ParseInteger(token.Data).transform(construct);
    case Lexer::TokenType::String:
        if (token.Data.empty() || token.Data.front() - '0' <= 9) return std::unexpected(ParseError::InvalidTokenData);
        
        registers = TryParseRegisters(token.Data);
        if (registers) return construct(registers.value());

        jump = TryParseJump(token.Data);
        if (jump) return construct(jump.value());

        return ParseVariable(token.Data).transform(construct);
    default:
        return std::unexpected(ParseError::InvalidTokenType);
    }
}

static constexpr bool IsUnary(Operations operation) {
    return operation == Operations::Negation || operation == Operations::Subtraction;
}

bool SemanticToken::ValidAfter(const SemanticToken& previous) const {
    if (previous.GetType() == Type::Jump) {
        return false;
    }

    if (previous.GetType() == Type::Operation) {
        return GetType() != Type::Operation || IsUnary(GetData<Operations>());
    }

    return GetType() == Type::Operation;
}
