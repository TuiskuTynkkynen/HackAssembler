#include "Parser.h"
#include "Log.h"
#include "CodeGeneration.h"

#include "IO/File.h"

int main() {
    auto input = IO::TryOpenFileInput("in.asm");
    if (!input.has_value()) {
        Log::Message("Could not open {} for reading", IO::GetAblsolutePath("in.asm"));
        return 1;
    }
    
    auto output = IO::TryOpenFileOutput("out.hack");
    if (!output.has_value()) {
        Log::Message("Could not open {} for writing", IO::GetAblsolutePath("out.hack"));
        return 1;
    }

    Log::Message("{} -> {}", IO::GetAblsolutePath("test.asm"), IO::GetAblsolutePath("out.hack"));

    auto code = input.and_then([](auto&& s) { return Parser::Parse(s); })
        .transform([](auto&& p) { return Parser::ParseResult::ResolveSymbols(p); })
        .transform(CodeGeneration::GenerateCode)
        .value();

    Log::SetOutput(std::move(output.value()));
    for (auto c : code) {
         Log::Message("{:0>16b}", c);
    }
}