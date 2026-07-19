#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() = default;

    std::int64_t get(std::string key) const;
    void set(std::string key, std::int64_t value);
private:
    std::unordered_map<std::string, std::int64_t> config_table;
};
