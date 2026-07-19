#include <CppUTest/TestHarness.h>
#include <stdexcept>

#include "config/config_manager.hpp"

TEST_GROUP(ConfigManagerTest)
{};

// キーに対応した値を取得できる
TEST(ConfigManagerTest, ReturnsValueForKey)
{
    ConfigManager manager;
    int64_t expected_width = 3;
    int64_t expected_height = 4;

    const auto width = manager.get("capture.width");
    const auto height = manager.get("capture.height");

    LONGS_EQUAL(expected_width, width);
    LONGS_EQUAL(expected_height, height);
}

// 存在しないキーの場合は例外が発生する
TEST(ConfigManagerTest, ThrowsWhenKeyDoesNotExist)
{
    ConfigManager manager;

    CHECK_THROWS(std::out_of_range, manager.get("unknown.key"));
}
