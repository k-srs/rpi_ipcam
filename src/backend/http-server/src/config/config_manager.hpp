#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

using ConfigValue = std::variant<std::int64_t, std::string>;

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() = default;

    ConfigValue get(std::string key) const;
    void set(std::string key, ConfigValue value);
private:
    std::unordered_map<std::string, ConfigValue> config_table;
};
