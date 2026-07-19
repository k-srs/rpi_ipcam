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
    return config_table.at(key);
}

void ConfigManager::set(std::string key, ConfigValue value)
{
    config_table.at(key) = std::move(value);
}
