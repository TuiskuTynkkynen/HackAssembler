#pragma once

#include "Lexer.h"
#include "SemanticToken.h"
#include "Instructions.h"

#include <print>

class Log {
public:
    template<typename... Args>
    static void Message(std::format_string<Args...> fmt, Args&&... args) {
        std::println(m_OutputStream, fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    static void InlineMessage(std::format_string<Args...> fmt, Args&&... args) {
        std::print(m_OutputStream, fmt, std::forward<Args>(args)...);
    }

    struct StreamState {
        std::istream& Stream;

        size_t LineNumber = 1;
        size_t LineLength = 0;
        size_t ExpressionOffset = (size_t)-1;
        size_t ExpressionLength = 0;
        
        size_t TokenOffset = (size_t)-1;
        size_t TokenLength = 0;
        
        StreamState(std::istream& stream) : Stream(stream) {}
        StreamState(std::istream& stream, size_t lineNumber, size_t lineLength, size_t expressionOffset, size_t expressionLength)
            : Stream(stream), LineNumber(lineNumber), LineLength(lineLength), ExpressionOffset(expressionOffset), ExpressionLength(expressionLength) {}
        
    };

    static void Error(Instructions::ParseError error, const StreamState& info);
    static void Error(SemanticToken::ParseError error, Lexer::TokenType tokenType, const StreamState& info);
    
    static void TokenError(SemanticToken::ParseError error, Lexer::TokenType tokenType);
    static void InstructionError(Instructions::ParseError error);

    static void ErrorLine(const StreamState& info);
    static void ErrorLineSimple(const StreamState& info);
    static void ErrorHeader(size_t lineNumber, size_t characterNumber);

    static void SetInputStreamName(std::string name) { m_StreamName = name; }
    static void SetOutput(std::FILE* stream) { m_OutputStream = stream; }
private:
    inline static std::FILE* m_OutputStream = stdout;
    inline static std::string m_StreamName = "Input Stream";
};

