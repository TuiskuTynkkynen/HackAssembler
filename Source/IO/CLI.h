#pragma once

#include <span>
#include <string>
#include <string_view>
#include <expected>

namespace CLI {
    struct Options {
        std::string InputFile;
        std::string OutputFile;

        struct ParseError {
            enum Types : uint8_t {
                MissingInput,
                UnknownArgument,
                TooManyArguments,
            };

            Types Type;
            std::string_view Message;
        };
    };

    std::expected<Options, Options::ParseError> ParseArguments(std::span<char*> arguments) {
        switch (arguments.size()) {
        case 1:
            return std::unexpected<Options::ParseError>(std::in_place, Options::ParseError::MissingInput, "missing input file");
        case 2: {
            std::string input = arguments[1];
            size_t extensionStart = input.find_last_of(".");
            extensionStart = extensionStart == std::string::npos ? input.length() : extensionStart;

            return std::expected<Options, Options::ParseError>(std::in_place, input, input.substr(0, extensionStart) + ".hack"); }
        case 3:
            return std::expected<Options, Options::ParseError>(std::in_place, arguments[1], arguments[2]);
        default:
            return std::unexpected<Options::ParseError>(std::in_place, Options::ParseError::TooManyArguments, "too many arguments");
        }
    }

    std::string_view GetUsge() {
        return "assembler <input_file> [output_file]";
    }
}