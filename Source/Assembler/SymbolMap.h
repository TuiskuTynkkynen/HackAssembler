#pragma once

#include <string>
#include <optional>
#include <unordered_map>

class SymbolMap {
public:
    SymbolMap();

    bool TryAddLabel(std::string name, uint16_t value);
    uint16_t AddVariable(std::string name);

    bool Contains(const std::string& name);
    uint16_t Get(const std::string& name);
    std::optional<uint16_t> TryGet(const std::string& name);
private:
    std::unordered_map<std::string, uint16_t> m_Symbols;
    uint16_t m_VariableAddress = 0x10;
};

