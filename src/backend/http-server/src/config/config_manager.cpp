#include <stdexcept>

#include "config/config_manager.hpp"

ConfigManager::ConfigManager() {
    config_table = {
        {"capture.width", 3},
        {"capture.height", 4},
    };
}

std::int64_t ConfigManager::get(std::string key) const
{
    auto it = config_table.find(key);
    if (it != config_table.end()) {
        return it->second;
    }
    throw std::out_of_range("key not found");
}
void ConfigManager::set(std::string key, std::int64_t value)
{
    auto it = config_table.find(key);
    if (it != config_table.end()) {
        it->second = value;
        return;
    }
    throw std::out_of_range("key not found");
}
