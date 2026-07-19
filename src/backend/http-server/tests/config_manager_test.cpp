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
    ConfigValue expected_width = 3;
    ConfigValue expected_height = 4;

    const auto width = manager.get("capture.width");
    const auto height = manager.get("capture.height");

    CHECK(expected_width == width);
    CHECK(expected_height == height);
}

// キーに対応した値を取得できる
TEST(ConfigManagerTest, ReturnsStringValueForKey)
{
    ConfigManager manager;
    ConfigValue expected_aspect_ratio = "16:9";

    const auto aspect_ratio = manager.get("capture.aspect_ratio");

    CHECK(expected_aspect_ratio == aspect_ratio);
}

// キーに対応した値を設定できる
TEST(ConfigManagerTest, SetValue)
{
    ConfigManager manager;
    ConfigValue expected_width = 4;

    manager.set("capture.width", expected_width);

    CHECK(expected_width == manager.get("capture.width"));
}

// 存在しないキーの設定値を取得した場合は例外が発生する
TEST(ConfigManagerTest, GetThrowsWhenKeyDoesNotExist)
{
    ConfigManager manager;

    CHECK_THROWS(std::out_of_range, manager.get("unknown.key"));
}

// 存在しないキーの設定値をセットした場合は例外が発生する
TEST(ConfigManagerTest, SetThrowsWhenKeyDoesNotExist)
{
    ConfigManager manager;

    CHECK_THROWS(std::out_of_range, manager.set("unknown.key", 0));
}
