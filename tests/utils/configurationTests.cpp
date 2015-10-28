#include <catch.hpp>
#include "ecs/ecs.h"

TEST_CASE("Configuration retrieval tests", "[Configuration]") {
    Configuration configuration;
    std::string sampleConfig = R"(
		--this is sample comment
		globalSetting = 1a
		sampleInteger = 123
		sampleBool = true
		sampleModule {
			nestedSetting = 1a2s3d4f--comment
			stringSetting = lorem ipsum dolor sit amet -- another comment
		}
	)";
    configuration.loadFromMemory(sampleConfig);

    SECTION("Existance tests") {
        REQUIRE(configuration.exists("globalSetting"));
        REQUIRE(configuration.exists("sampleInteger"));
        REQUIRE(configuration.exists("sampleBool"));
        REQUIRE(configuration.exists("sampleModule.nestedSetting"));
        REQUIRE(configuration.exists("sampleModule.stringSetting"));
    }

    SECTION("Global settings") {
        REQUIRE(configuration.get("globalSetting") == "1a");
    }

    SECTION("Nested settings") {
        REQUIRE(configuration.get("sampleModule.nestedSetting") == "1a2s3d4f");
        REQUIRE(configuration.get("sampleModule.stringSetting") == "lorem ipsum dolor sit amet");
    }

    SECTION("Non-existant settings") {
        //returns default-constructed variable if fallback value not supplied
        REQUIRE(configuration.get("non-existantSetting") == "");
        REQUIRE(configuration.get<int>("zero") == 0);

        //returns fallback value if supplied
        REQUIRE(configuration.get("non-ExistantModule.stringSetting", "someSettingVal") == "someSettingVal");
        REQUIRE(configuration.get<int>("answer", 42) == 42);

        REQUIRE(!configuration.exists("non-ExistantModule.stringSetting"));
        REQUIRE(!configuration.exists("answer"));
    }

    SECTION("Settings conversions") {
        int sampleInteger = configuration.get<int>("sampleInteger");
        REQUIRE(sampleInteger == 123);
    }

    SECTION("Settings default values") {
        int numberOfTheBeast = configuration.get("numOfBeast", 666);
        REQUIRE(numberOfTheBeast == 666);
    }
}

TEST_CASE("Configuration set() test", "[Configuration]") {
    Configuration configuration;
    configuration.set("custom.some.module.answer", 42);
    REQUIRE(configuration.get<int>("custom.some.module.answer") == 42);
}

TEST_CASE("Configuration serialization test", "[Configuration]") {
    Configuration configuration;

    //load initial config
    std::string testConfig = "testSetting = asdf\nanotherSetting = 1234\ntestModule {\n\tnestedSetting = 54321\n}\n\n";
    configuration.loadFromMemory(testConfig);

    //serialize initial config
    std::string serializedConfig = configuration.serializeConfig();

    //reload the same config, this time from serialized data
    configuration.clear();
    configuration.loadFromMemory(serializedConfig);
    
    //check if content is the same
    REQUIRE(configuration.get("testSetting") == "asdf");
    REQUIRE(configuration.get<int>("anotherSetting") == 1234);
    REQUIRE(configuration.get<int>("testModule.nestedSetting") == 54321);
}

