#include <stdexcept>

#include "config/config_manager.hpp"

ConfigManager::ConfigManager() {
    config_table = {
        {"capture.width", std::int64_t{3}},
        {"capture.height", std::int64_t{4}},
        {"capture.aspect_ratio", std::string{"16:9"}},
    };
}

ConfigValue ConfigManager::get(std::string key) const
{
    auto it = config_table.find(key);
    if (it != config_table.end()) {
        return it->second;
    }
    throw std::out_of_range("key not found");
}
void ConfigManager::set(std::string key, ConfigValue value)
{
    auto it = config_table.find(key);
    if (it != config_table.end()) {
        it->second = value;
        return;
    }
    throw std::out_of_range("key not found");
}
