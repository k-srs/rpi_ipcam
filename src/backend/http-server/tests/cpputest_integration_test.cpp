#include <CppUTest/TestHarness.h>

TEST_GROUP(CppUTestIntegration)
{};

TEST(CppUTestIntegration, RunsRegisteredTests)
{
    CHECK_TRUE(true);
}
