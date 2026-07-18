#include "config_manager.hpp"

#include "config/config_manager.hpp"

std::int64_t ConfigManager::get(std::string_view key) const
{
    (void)key; // 未使用エラー解消のため
    return 3;
}
