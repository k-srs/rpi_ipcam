#pragma once

#include <cstdint>
#include <string>

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() = default;

    std::int64_t get(std::string key) const;
    void set(std::string key, std::int64_t value);
};
