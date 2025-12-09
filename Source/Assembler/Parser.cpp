#include "Parser.h"

#include "Lexer.h"
#include "SemanticToken.h"
#include "IO/Log.h"

#include <expected>
#include <istream>
#include <numeric>

class DebugState {
public:
    Log::StreamState StreamInfo;
    std::vector<Lexer::Token> TokenStack;

    DebugState(std::istream& stream) : StreamInfo(stream) {}

    void ResetExpression() {
        StreamInfo.ExpressionOffset = StreamInfo.TokenOffset = (size_t)-1;
        StreamInfo.ExpressionLength = StreamInfo.TokenLength = 0;
        TokenStack.clear();
    }

    void AddLine() {
        ResetExpression();
        StreamInfo.LineLength = 0;
        StreamInfo.LineNumber++;
    }

    void ExpressionAdd(const Lexer::Token& token) {
        TokenStack.push_back(token);
        if (StreamInfo.ExpressionOffset != (size_t)-1) {
            StreamInfo.ExpressionLength += token.CharactersConsumed;
            return;
        }

        StreamInfo.ExpressionOffset += StreamInfo.LineLength - token.Data.length() + 1;
        StreamInfo.ExpressionLength += token.Data.length();
    }
};

static constexpr bool CollapseStack(std::vector<SemanticToken>& semanticStack, std::vector<std::variant<AddressingInstruction, ComputeInstruction>>& instructions, DebugState& debug) {
    if (semanticStack.empty()) return false;


    std::expected<std::variant<AddressingInstruction, ComputeInstruction>, Instructions::ParseError> result;
    if (semanticStack.size() == 1) {
        result = AddressingInstruction::Create(semanticStack.front());
    }
    else {
        result = ComputeInstruction::Create(semanticStack);
    }

    semanticStack.clear();

    if (result.has_value()) {
        instructions.emplace_back(result.value());
        debug.ResetExpression();

        return false;
    }

    const auto& error = result.error();
    if (error.StackIndex < debug.TokenStack.size()) {
        debug.StreamInfo.TokenLength = debug.TokenStack.front().Data.length();

        debug.StreamInfo.TokenOffset = debug.StreamInfo.ExpressionOffset;
        
        if (error.StackIndex != 0) {
            auto fold = [](size_t lhs, const Lexer::Token& rhs) { return lhs + rhs.CharactersConsumed; };
            debug.StreamInfo.TokenOffset += std::accumulate(debug.TokenStack.begin() + 1, debug.TokenStack.begin() + error.StackIndex, (size_t)0, fold);
            debug.StreamInfo.TokenOffset += debug.TokenStack[error.StackIndex].CharactersConsumed;
        }
    }

    Log::Error(error, debug.StreamInfo);
    debug.ResetExpression();
    return true;
};

std::optional<Parser::ParseResult> Parser::Parse(std::istream& stream) {
    std::vector<std::variant<AddressingInstruction, ComputeInstruction>> instructions;
    SymbolMap symbols;

    std::vector<SemanticToken> semanticStack;
    bool errors = false;

    DebugState debug(stream);
    for (bool done = false; !done;) {
        std::expected<SemanticToken, SemanticToken::ParseError> result(std::unexpect, SemanticToken::ParseError::Unexpected);
        Lexer::Token token = Lexer::GetNextToken(stream);

        debug.StreamInfo.LineLength += token.CharactersConsumed;
        
        switch (token.Type) {
            using enum Lexer::TokenType::Value;
        case EndOfStream:
            errors |= CollapseStack(semanticStack, instructions, debug);
            done = true;
            continue;
        case Comment:
            continue;
        case Label:
            errors |= CollapseStack(semanticStack, instructions, debug);

            token.Type = Lexer::TokenType::String;
            result = SemanticToken::Create(token);
            if (result.has_value() && result.value().GetType() == SemanticToken::Type::Variable) {
                if (symbols.TryAddLabel(token.Data, static_cast<uint16_t>(instructions.size()))) {
                    continue;
                }
                
                Log::ErrorHeader(debug.StreamInfo.LineNumber, debug.StreamInfo.LineLength);
                Log::Message("Label has already been defined");
                Log::ErrorLine(Log::StreamState{ stream, debug.StreamInfo.LineNumber, debug.StreamInfo.LineLength, debug.StreamInfo.LineLength - token.Data.length() - 1, token.Data.length() });

                errors = true;
                continue;
            } 

            Log::Error(result.error(), Lexer::TokenType::Label, Log::StreamState{ stream, debug.StreamInfo.LineNumber, debug.StreamInfo.LineLength, debug.StreamInfo.LineLength - token.Data.length() - 1, token.Data.length() });
            continue;
        case Newline:
            debug.StreamInfo.LineLength++;
            errors |= CollapseStack(semanticStack, instructions, debug);
            debug.AddLine();
            continue;
	    default: 
            break;
        }

        result = SemanticToken::Create(token);
        if (!result.has_value()) {
            size_t offset = debug.StreamInfo.LineLength - token.Data.length();
            Log::Error(result.error(), token.Type, Log::StreamState{stream, debug.StreamInfo.LineNumber, debug.StreamInfo.LineLength, offset, token.Data.length()});
            continue;
        }

        bool hasWhiteSpace = token.CharactersConsumed != token.Data.length();
        if (!semanticStack.empty() && hasWhiteSpace && !result.value().ValidAfter(semanticStack.back())) {
            errors |= CollapseStack(semanticStack, instructions, debug);
        }

        semanticStack.emplace_back(result.value());
        debug.ExpressionAdd(token);
    }

    return errors ? std::optional<ParseResult>(std::nullopt) : std::optional<ParseResult>(std::in_place, instructions, symbols);
}

std::vector<std::variant<LoadInstruction, ComputeInstruction>> Parser::ParseResult::ResolveSymbols(ParseResult& parsed) {
     auto transform = [&parsed](const auto& instruction) {
        if constexpr (std::is_convertible_v<ComputeInstruction, decltype(instruction)>) {
            return std::variant<LoadInstruction, ComputeInstruction>(instruction);
        } else {
            auto variable = std::get_if<std::string>(&instruction.Variable);

            if (!variable) {
                return std::variant<LoadInstruction, ComputeInstruction>(LoadInstruction{ std::get<uint16_t>(instruction.Variable) });
            }

            uint16_t value = parsed.Symbols.TryGet(*variable)
                .or_else([&] { return std::optional<uint16_t>(parsed.Symbols.AddVariable(*variable)); })
                .value();

            return std::variant<LoadInstruction, ComputeInstruction>(LoadInstruction{ value });
        }};

    std::vector<std::variant<LoadInstruction, ComputeInstruction>> result;
    result.reserve(parsed.Instructions.size());

    for (const auto& instruction : parsed.Instructions) {
        result.emplace_back(std::visit(transform, instruction));
    }

    return result;
}
