#include <CppUTest/TestHarness.h>

#include "config/config_manager.hpp"

TEST_GROUP(ConfigManagerTest)
{};

TEST(ConfigManagerTest, ReturnsValueForKey)
{
    ConfigManager manager;
    int64_t expected = 3;

    const auto width = manager.get("capture.width");

    LONGS_EQUAL(expected, width);
}
