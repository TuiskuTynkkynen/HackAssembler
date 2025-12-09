#include "Log.h"

#include <istream>

constexpr auto ANSI_COLOR_RESET= "\033[0m";
constexpr auto ANSI_COLOR_RED = "\033[31m";
constexpr auto ANSI_COLOR_CYAN = "\033[36m";
constexpr auto ANSI_COLOR_GRAY = "\033[90m";

void Log::Error(Instructions::ParseError error, const StreamState& info) {
    ErrorHeader(info.LineNumber, info.ExpressionOffset);
    InstructionError(error);
    ErrorLine(info);
}

void Log::Error(SemanticToken::ParseError error, Lexer::TokenType tokenType, const StreamState& info) {
    ErrorHeader(info.LineNumber, info.ExpressionOffset);
    TokenError(error, tokenType);;
    ErrorLineSimple(info);
}

void Log::TokenError(SemanticToken::ParseError error, Lexer::TokenType type) {
    switch (error) {
        using enum SemanticToken::ParseError;
    case Unexpected:
        Message("Unexpected error parsing token of type {}", type.ToString());
        break;
    case InvalidTokenType:
        Message("Can not parse token of type {}", type.ToString());
        break;
    case InvalidTokenData:
        Message("Token is not a valid {}", type.ToString());
        break;
    case IntegerOutOfRange:
        Message("Token was out of range for 16 bit unsigned integer");
        break;
    }
}

void Log::InstructionError(Instructions::ParseError error) {
    switch (error.Type) {
        using enum Instructions::ParseError::Types;
    case InvalidSemanticTokenType:
        Message("Expression could not be parsed as a addressing instruction");
        break;
    case InvalidSemanticTokenCount:
    case InvalidOperationOrder:
    case MissingOperation:
    case MissingOperand:
    case InvalidAssignmentOrder:
    case InvalidDestination:
    case InvalidOperand:
    case InvalidJumpOrder:
    case InvalidJump:
    case MissingEffect:
    case InvalidRegisterOperandOrder:
    case InvalidRegisterOperands:
    case InvalidNumericOperand:
    case InvalidOneOperand:
    case InvalidZeroOperand:
        break;
    }
}

static std::string GetLine(std::istream& stream, size_t lineLength) {
    std::streampos current = stream.tellg();

    std::streampos offset = -std::min(static_cast<std::streampos>(lineLength), current);
    stream.seekg(offset, std::ios::cur);

    std::string result;
    std::getline(stream, result);

    stream.seekg(current);

    return result;
}

static std::string RemoveTabs(std::span<const char> chars) {
    std::string result;
    result.reserve(chars.size());

    for (size_t i = 0; i < chars.size(); i++) {
        if (chars[i] != '\t') {
            result.push_back(chars[i]);
            continue;
        }

        result.append("    ", 4 - i % 4);
    }
    return result;
}


void Log::ErrorLine(const StreamState& info) {
    if (info.TokenOffset == (size_t)-1 ||  info.TokenLength == info.ExpressionLength) {
        ErrorLineSimple(info);
        return;
    }
    
    assert(info.TokenLength < info.ExpressionLength);
    assert(info.TokenOffset >= info.ExpressionOffset);
    

    auto line = GetLine(info.Stream, info.LineLength);
    std::string before = RemoveTabs({ line.data(), info.ExpressionOffset });
    std::string expression = RemoveTabs({line.data() + info.ExpressionOffset, info.TokenOffset - info.ExpressionOffset});
    std::string after = RemoveTabs({ line.data() + info.TokenOffset, info.ExpressionOffset + info.ExpressionLength - (info.TokenOffset) });

    Message("{}{}{}{}", before, expression, after, std::string_view(line.begin() + info.ExpressionOffset + info.ExpressionLength, line.end()));
    InlineMessage("{}{}{}", std::string(before.length(), ' '), ANSI_COLOR_GRAY, std::string(expression.length() + after.length(), '~'));
    Message("\033[{}D{}{}{}", after.length(), ANSI_COLOR_RED, std::string(info.TokenLength, '~'), ANSI_COLOR_RESET);
}

void Log::ErrorLineSimple(const StreamState& info) {
    auto line = GetLine(info.Stream, info.LineLength);

    std::string before = RemoveTabs({ line.data(), info.ExpressionOffset });
    std::string expression = RemoveTabs({ line.data() + info.ExpressionOffset, info.ExpressionLength });

    Message("{}{}{}", before, expression, std::string_view(line.begin() + info.ExpressionOffset + info.ExpressionLength, line.end()));
    Message("{}{}{}{}", std::string(before.length(), ' '), ANSI_COLOR_RED, std::string(expression.length(), '~'), ANSI_COLOR_RESET);
}

void Log::ErrorHeader(size_t lineNumber, size_t characterNumber) {
    InlineMessage("{}:{}{}{}:{}{}{} ", m_StreamName, ANSI_COLOR_CYAN, lineNumber, ANSI_COLOR_RESET, ANSI_COLOR_CYAN, characterNumber, ANSI_COLOR_RESET);
}