#include "Assembler/Parser.h"
#include "Assembler/CodeGeneration.h"

#include "IO/Log.h"
#include "IO/File.h"
#include "IO/CLI.h"

int main(int argc, char** argv) {
    auto options = CLI::ParseArguments({ argv, static_cast<size_t>(argc) });
    if (!options.has_value()) {
        Log::Message("assembler: {}", options.error().Message);
        Log::Message("usage: {}", CLI::GetUsge());
        return 1;
    }

    Log::SetInputStreamName(options.value().InputFile);
    auto input = IO::TryOpenFileInput(options.value().InputFile);
    if (!input.has_value()) {
        Log::Message("Could not open {} for reading", IO::GetAblsolutePath(options.value().InputFile));
        return 1;
    }

    auto output = IO::TryOpenFileOutput(options.value().OutputFile);
    if (!output.has_value()) {
        Log::Message("Could not open {} for writing", IO::GetAblsolutePath(options.value().OutputFile));
        return 1;
    }

    auto code = input.and_then([](auto&& s) { return Parser::Parse(s); })
        .transform([](auto&& p) { return Parser::ParseResult::ResolveSymbols(p); })
        .transform(CodeGeneration::GenerateCode);

    if (!code.has_value()) {
        return 1;
    }

    Log::SetOutput(std::move(output.value()));
    for (uint16_t c : code.value()) {
        Log::Message("{:0>16b}", c);
    }

    Log::ResetOutput();
    Log::Message("{} -> {}", IO::GetAblsolutePath(options.value().InputFile), IO::GetAblsolutePath(options.value().OutputFile));
}