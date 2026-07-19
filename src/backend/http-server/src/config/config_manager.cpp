#include <stdexcept>

#include "config/config_manager.hpp"

std::int64_t ConfigManager::get(std::string_view key) const
{
    if (key == "capture.width") {
        return 3;
    } else if (key == "capture.height") {
        return 4;
    }
    throw std::out_of_range("key does not found");
}
