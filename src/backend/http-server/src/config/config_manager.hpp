#pragma once

#include <cstdint>
#include <string_view>

class ConfigManager {
public:
    std::int64_t get(std::string_view key) const;
};
