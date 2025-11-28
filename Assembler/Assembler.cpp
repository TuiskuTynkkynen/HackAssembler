#include "Parser.h"
#include "Log.h"

#include <sstream>

int main() {
    std::stringstream stream("@35         // A_register holds 35\nD = A       // D_register holds 35\n@100        // A_register holds 100\nD = D + A   // D_register holds 135\n");
    
    auto parseResult = Parser::Parse(stream);

    if (!parseResult.has_value()) {
        return 1;
    }

    for (const auto& instruction : parseResult.value().Instructions) {
        std::visit([](const auto& ins) { Log::Message("{}", ins.ToString()); }, instruction);
    }
}