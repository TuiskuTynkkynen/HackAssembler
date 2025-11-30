#include "Parser.h"
#include "Log.h"
#include "CodeGeneration.h"

#include <sstream>

int main() {
    std::stringstream stream(
"// Adds 1 + ... + 100              \n\
    @i                              \n\
    M=1 // i=1                      \n\
    @sum                            \n\
    M=0 // sum=0                    \n\
(LOOP)                              \n\
    @i                              \n\
    D=M // D=i                      \n\
    @100                            \n\
    D=D-A // D=i-100                \n\
    @END                            \n\
    D;JGT // if (i-100)>0 goto END  \n\
    @i                              \n\
    D=M // D=i                      \n\
    @sum                            \n\
    M=D+M // sum=sum+i              \n\
    @i                              \n\
    M=M+1 // i=i+1                  \n\
    @LOOP                           \n\
    0;JMP // goto LOOP              \n\
(END)                               \n\
    @END                            \n\
    0;JMP // infinite loop");

    auto parseResult = Parser::Parse(stream);

    if (!parseResult.has_value()) {
        return 1;
    }

    for (const auto& instruction : parseResult.value().Instructions) {
        std::visit([](const auto& ins) { Log::Message("{}", ins.ToString()); }, instruction);
    }

    Log::Message("");
    Log::Message("Symbols resolved");
    Log::Message("");

    auto resolved = Parser::ParseResult::ResolveSymbols(parseResult.value());
    for (const auto& instruction : resolved) {
        std::visit([](const auto& ins) { Log::Message("{}", ins.ToString()); }, instruction);
    }

    Log::Message("");
    Log::Message("Machine code");
    Log::Message("");

    auto code = CodeGeneration::GenerateCode(resolved);
    for (auto c : code) {
         Log::Message("{:0>16b}", c);
    }
}