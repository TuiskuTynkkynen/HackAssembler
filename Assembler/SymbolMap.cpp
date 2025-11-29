#include "SymbolMap.h"

#include <format>

SymbolMap::SymbolMap() {
    // Predefined symbolss
    m_Symbols.insert({
        { "SP",     0x0 },
        { "LCL",    0x1 },
        { "ARG",    0x2 },
        { "THIS",   0x3 },
        { "THAT",   0x4 },
        { "SCREEN", 0x4000},
        { "KBD",    0x6000},
    });

    for (uint16_t i = 0; i < 16; i++) {
        m_Symbols.emplace(std::format("R{}", i), i);
    }
}

bool SymbolMap::TryAddLabel(std::string name, uint16_t value) {
    bool result = !Contains(name);

    m_Symbols.try_emplace(name, value);

    return result;
}

uint16_t SymbolMap::AddVariable(std::string name) {
    if (!Contains(name)) {
        m_Symbols.emplace(name, m_VariableAddress);
        return m_VariableAddress++;
    }
    
    return Get(name);
}

bool SymbolMap::Contains(const std::string& name) {
    return m_Symbols.contains(name);
}

uint16_t SymbolMap::Get(const std::string& name) {
    return m_Symbols.at(name);
}

std::optional<uint16_t> SymbolMap::TryGet(const std::string& name) {
    auto iter = m_Symbols.find(name);

    return iter != m_Symbols.end() ? std::optional<uint16_t>(iter->second) : std::optional<uint16_t>{};
}