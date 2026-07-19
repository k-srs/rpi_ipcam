#include <stdexcept>

#include "config/config_manager.hpp"

// CppUTestが定義するnewマクロと標準ライブラリの干渉を避けるため、最後に読み込む
#include <CppUTest/TestHarness.h>

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

// キーに対応した値を設定できる
TEST(ConfigManagerTest, SetValue)
{
    ConfigManager manager;
    int64_t expected_width = 4;

    manager.set("capture.width", 4);
    const auto width = manager.get("capture.width");

    LONGS_EQUAL(expected_width, width);
}

// 存在しないキーの場合は例外が発生する
TEST(ConfigManagerTest, ThrowsWhenKeyDoesNotExist)
{
    ConfigManager manager;

    CHECK_THROWS(std::out_of_range, manager.get("unknown.key"));
    CHECK_THROWS(std::out_of_range, manager.set("unknown.key", 0));
}
