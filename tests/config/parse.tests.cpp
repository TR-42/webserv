#include <libgen.h>
#include <sys/param.h>

#include <config/ListenConfig.hpp>
#include <config/parseListenConfig.hpp>
#include <service/RequestedFileInfo.hpp>
#include <yaml/YamlParser.hpp>

#include "gtest/gtest.h"

namespace webserv
{

class ParseTests : public ::testing::Test
{
 private:
	std::string _testFileDir;

 protected:
	Logger logger;

 public:
	ParseTests();

	inline const std::string &getTestFileDir() const
	{
		return this->_testFileDir;
	}
};

ParseTests::ParseTests(
) : logger()
{
	char buf[MAXPATHLEN];
	if (realpath(__FILE__, buf) == nullptr) {
		abort();
	}

	this->_testFileDir = dirname(buf);
	this->_testFileDir += +"/../../resources/";
	if (realpath(this->_testFileDir.c_str(), buf) == nullptr) {
		abort();
	}
	this->_testFileDir = buf;

	CS_INFO() << "Test file dir: " << this->_testFileDir << std::endl;
}

TEST_F(ParseTests, parseListenConfig)
{
	std::vector<std::string> inputYaml = {
		"server1:",
		"  serverNameList:",
		"    localhost:",
		"    42.fr:",
		"  port: 8080",
		"  timeoutMs: 100",
		"  requestBodyLimit: 100",
		"  errorPages:",
		"    400: " + this->getTestFileDir() + "/sample1/400.html",
		"    404: " + this->getTestFileDir() + "/sample1/404.html",
		"  routeList:",
		"    route1:",
		"      request_path: /",
		"      methods:",
		"        GET:",
		"        POST:",
		"      document_root: " + this->getTestFileDir() + "/sample1",
		"      document_listing: true",
		"      index_files:",
		"        index.html",
		"        index.htm",
		"      cgi:",
		"        cgi1:",
		"          extensionWithoutDot: php",
		"          cgiExecutableFullPath: /usr/bin/php-cgi",
		"          envPreset:",
		"            PATH: /usr/bin:/bin",
		"        cgi2:",
		"          extensionWithoutDot: py",
		"          cgiExecutableFullPath: /usr/bin/python3",
		"          envPreset:",
		"            PATH: /usr/bin:/bin",
		"    route2:",
		"      request_path: /test",
		"      methods:",
		"        GET:",
		"      document_root: " + this->getTestFileDir() + "/sample2",
		"      document_listing: false",
		"      index_files:",
		"        index.html",
		"        index.htm",
		"      cgi:",
		"        cgi1:",
		"          extensionWithoutDot: php",
		"          cgiExecutableFullPath: /usr/bin/php-cgi",
		"          envPreset:",
		"            PATH: /usr/bin:/bin",
		"        cgi2:",
		"          extensionWithoutDot: py",
		"          cgiExecutableFullPath: /usr/bin/python3",
		"          envPreset:",
		"            PATH: /usr/bin:/bin",
		"    minimalRoute:",
		"      request_path: /minimal",
		"      document_root: " + this->getTestFileDir() + "/sample3",
		"    minimalRedirect:",
		"      request_path: /minimalRedirect",
		"      redirect:",
		"        to: /minimal",
		"        code: 301",
		"server2:",
		"  serverNameList:",
		"    42Tokyo.jp:",
		"  port: 8080",
		"  timeoutMs: 100",
		"  routeList:",
		"    route2-1:",
		"      request_path: /",
		"      document_root: " + this->getTestFileDir() + "/sample4",
		"server3:",
		"  serverNameList:",
		"    42Tokyo.jp:",
		"  port: 8081",
		"  timeoutMs: 100",
		"  routeList:",
		"    route3-1:",
		"      request_path: /",
		"      document_root: " + this->getTestFileDir() + "/sample4",
	};

	yaml::MappingNode root("");
	EXPECT_TRUE(yaml::parse(inputYaml, root, this->logger));
	EXPECT_EQ(root.getNodes().size(), 3);

	ListenConfig listenConfig;
	EXPECT_NO_THROW(
		listenConfig = parseListenConfig(root, this->getTestFileDir() + "/sample.yaml")
	);

	EXPECT_EQ(listenConfig.getListenMap().size(), 2);
	EXPECT_EQ(listenConfig.getListenMap().at(8080).size(), 2);
	EXPECT_EQ(listenConfig.getListenMap().at(8081).size(), 1);

	{
		HttpRouteConfig route2_1(
			"/",
			std::vector<std::string>(),
			HttpRedirectConfig(),
			this->getTestFileDir() + "/sample4",
			false,
			std::vector<std::string>(),
			std::vector<CgiConfig>()
		);

		ServerConfig server2(
			std::vector<std::string>{"42Tokyo.jp"},
			8080,
			100,
			0,
			ErrorPageMapType(),
			std::vector<HttpRouteConfig>{route2_1}
		);

		EXPECT_EQ(listenConfig.getListenMap().at(8080).at(1), server2);
	}

	{
		HttpRouteConfig route3_1(
			"/",
			std::vector<std::string>(),
			HttpRedirectConfig(),
			this->getTestFileDir() + "/sample4",
			false,
			std::vector<std::string>(),
			std::vector<CgiConfig>()
		);

		ServerConfig server3(
			std::vector<std::string>{"42Tokyo.jp"},
			8081,
			100,
			0,
			ErrorPageMapType(),
			std::vector<HttpRouteConfig>{route3_1}
		);

		EXPECT_EQ(listenConfig.getListenMap().at(8081).at(0), server3);
	}
}

}	 // namespace webserv
