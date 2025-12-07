#pragma once

#include <fstream>
#include <optional>
#include <filesystem>

namespace IO {
    [[nodiscard]] std::optional<std::ifstream> TryOpenFileInput(const std::string& fileName) {
        std::ifstream file;
        file.open(fileName);
        
        return file.good() ? std::optional<std::ifstream>(std::move(file)) : std::optional<std::ifstream>(std::nullopt);
    }
    
    [[nodiscard]] std::optional<std::ofstream> TryOpenFileOutput(const std::string& fileName) {
        std::ofstream file;
        file.open(fileName);
        
        return file.good() ? std::optional<std::ofstream>(std::move(file)) : std::optional<std::ofstream>(std::nullopt);
    }

    [[nodiscard]] std::string GetAblsolutePath(std::string_view path) {
        return std::filesystem::absolute(path).string();
    }
}