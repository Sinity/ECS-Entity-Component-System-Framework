#include <catch.hpp>
#include "ecs/utils/config.h"

TEST_CASE("Configuration read tests", "[Configuration]") {
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

	configuration.loadFromMemory(sampleConfig.c_str());

	SECTION("Global settings") {
		REQUIRE(configuration.get("globalSetting") == "1a");
	}

	SECTION("Nested settings") {
		REQUIRE(configuration.get("sampleModule.nestedSetting") == "1a2s3d4f");
		REQUIRE(configuration.get("sampleModule.stringSetting") == "lorem ipsum dolor sit amet");
	}

	SECTION("Non-existant settings without default values") {
		REQUIRE(configuration.get("non-existingSetting") == "");
		REQUIRE(configuration.get("non-ExistingModule.stringSetting") == "");

		int zero = configuration.get<int>("zero");
		REQUIRE(zero == 0);
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
