#define BOOST_TEST_MODULE Config
#include <boost/test/unit_test.hpp>
#include "tool/logger.h"
#include "tool/config.h"

BOOST_AUTO_TEST_CASE(configuration_test) {
    //init configuration system
	Logger logger;
	Configuration configuration(logger);
    std::string sampleConfig = R"(--this is sample comment
								main {
									globalSetting = 1a
									sampleInteger = 123
									sampleBool = true
									sampleModule {
										nestedSetting = 1a2s3d4f--comment
										stringSetting = lorem ipsum dolor sit amet -- another comment
									}
								}
								)";

    configuration.loadFromMemory(sampleConfig.c_str());

    //test global settings
    BOOST_CHECK_EQUAL(configuration.get("globalSetting"), "1a");
    BOOST_CHECK_EQUAL(configuration.get("globalSetting").size(), 2);
    BOOST_CHECK_EQUAL(configuration.get("globalSetting").c_str(), configuration.get("main.globalSetting", std::string()).c_str()); // check if it's the same setting

    //test nested settings
    BOOST_CHECK_EQUAL(configuration.get("sampleModule.nestedSetting"), "1a2s3d4f");
    BOOST_CHECK_EQUAL(configuration.get("sampleModule.stringSetting"), "lorem ipsum dolor sit amet");

    //test behaviour if requesting non existing settings
    BOOST_CHECK_EQUAL(configuration.get("non-existingSetting"), "");
    BOOST_CHECK_EQUAL(configuration.get("non-ExistingModule.stringSetting"), "");

    //test converstions
    int sampleInteger = configuration.get<int>("sampleInteger");
    BOOST_CHECK_EQUAL(sampleInteger, 123);

    //test default values
    int numberOfTheBeast = configuration.get("numOfBeast", 666);
    BOOST_CHECK_EQUAL(numberOfTheBeast, 666);

    int zero = configuration.get<int>("zero");
    BOOST_CHECK_EQUAL(zero, 0);

    std::string empty = configuration.get("non-existing-setting");
    BOOST_CHECK_EQUAL(empty, std::string());
}
