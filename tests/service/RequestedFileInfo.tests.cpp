#include <gtest/gtest.h>
#include <libgen.h>
#include <stdlib.h>
#include <sys/param.h>

#include <iostream>
#include <service/RequestedFileInfo.hpp>

namespace webserv
{

class RequestedFileInfoTests : public ::testing::Test
{
 private:
	std::string _testFileDir;
	HttpRouteConfig _routeConfig;
	HttpRouteConfig _routeConfigIndexEnabled;
	HttpRouteConfig _routeConfigCgi;

 protected:
	Logger logger;

 public:
	RequestedFileInfoTests();

	inline const std::string &getTestFileDir() const
	{
		return this->_testFileDir;
	}

	inline const HttpRouteConfig &getRouteConfig() const
	{
		return this->_routeConfig;
	}

	inline const HttpRouteConfig &getRouteConfigIndexEnabled() const
	{
		return this->_routeConfigIndexEnabled;
	}

	inline const HttpRouteConfig &getRouteConfigCgi() const
	{
		return this->_routeConfigCgi;
	}
};

RequestedFileInfoTests::RequestedFileInfoTests(
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

	this->_routeConfig.setRequestPath("/test/path");
	this->_routeConfig.setDocumentRoot(this->getTestFileDir());
	this->_routeConfig.setIsDocumentListingEnabled(false);
	this->_routeConfig.setIndexFileList({"index.html", "index.php"});

	this->_routeConfigIndexEnabled = this->_routeConfig;
	this->_routeConfigIndexEnabled.setIsDocumentListingEnabled(true);

	CgiConfig cgiConfig;
	cgiConfig.setCgiExecutableFullPath("/usr/bin/php-cgi");
	cgiConfig.setExtensionWithoutDot("php");
	this->_routeConfigCgi = this->_routeConfig;
	this->_routeConfigCgi.setCgiConfigList({cgiConfig});
}

TEST_F(RequestedFileInfoTests, SIZE_MAX)
{
	EXPECT_EQ(SIZE_MAX, 18446744073709551615UL);
}

TEST_F(RequestedFileInfoTests, tooFewPathSeg)
{
	RequestedFileInfo requestedFileInfo({"test", "path"}, false, this->getRouteConfig(), this->logger);

	EXPECT_TRUE(requestedFileInfo.getIsNotFound());
}

TEST_F(RequestedFileInfoTests, testDocumentRoot)
{
	RequestedFileInfo requestedFileInfo({"test", "path"}, true, this->getRouteConfig(), this->logger);

	// 本当はファイルじゃ無いのに入っていてほしくはないが、その対応は面倒なのでこのまま。
	// 結局CGIにならないと意味をなさないので、このままで良い。
	EXPECT_EQ("/test/path", requestedFileInfo.getCgiScriptName());
	EXPECT_EQ("", requestedFileInfo.getCgiPathInfo());
	EXPECT_EQ("", requestedFileInfo.getCgiPathTranslated());
	EXPECT_EQ("", requestedFileInfo.getTargetFilePathWithoutDocumentRoot());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getTargetFilePath());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getDocumentRoot());

	EXPECT_TRUE(requestedFileInfo.getIsDirectory());
	EXPECT_TRUE(requestedFileInfo.getIsNotFound());
	EXPECT_FALSE(requestedFileInfo.getIsCgi());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexAllowed());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexFile());

	EXPECT_EQ("", requestedFileInfo.getFileExtensionWithoutDot());
}

TEST_F(RequestedFileInfoTests, testDocumentRootAutoIndex)
{
	RequestedFileInfo requestedFileInfo({"test", "path"}, true, this->getRouteConfigIndexEnabled(), this->logger);

	// 本当はファイルじゃ無いのに入っていてほしくはないが、その対応は面倒なのでこのまま。
	// 結局CGIにならないと意味をなさないので、このままで良い。
	EXPECT_EQ("/test/path", requestedFileInfo.getCgiScriptName());
	EXPECT_EQ("", requestedFileInfo.getCgiPathInfo());
	EXPECT_EQ("", requestedFileInfo.getCgiPathTranslated());
	EXPECT_EQ("", requestedFileInfo.getTargetFilePathWithoutDocumentRoot());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getTargetFilePath());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getDocumentRoot());

	EXPECT_TRUE(requestedFileInfo.getIsDirectory());
	EXPECT_FALSE(requestedFileInfo.getIsNotFound());
	EXPECT_FALSE(requestedFileInfo.getIsCgi());
	EXPECT_TRUE(requestedFileInfo.getIsAutoIndexAllowed());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexFile());

	EXPECT_EQ("", requestedFileInfo.getFileExtensionWithoutDot());
}

TEST_F(RequestedFileInfoTests, detectIndexHtml)
{
	RequestedFileInfo requestedFileInfo({"test", "path", "www"}, false, this->getRouteConfig(), this->logger);

	EXPECT_EQ("index.html", requestedFileInfo.getFileName());
	EXPECT_EQ("/test/path/www/index.html", requestedFileInfo.getCgiScriptName());
	EXPECT_EQ("", requestedFileInfo.getCgiPathInfo());
	EXPECT_EQ("", requestedFileInfo.getCgiPathTranslated());
	EXPECT_EQ("/www/index.html", requestedFileInfo.getTargetFilePathWithoutDocumentRoot());
	EXPECT_EQ(this->getTestFileDir() + "/www/index.html", requestedFileInfo.getTargetFilePath());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getDocumentRoot());

	EXPECT_FALSE(requestedFileInfo.getIsDirectory());
	EXPECT_FALSE(requestedFileInfo.getIsNotFound());
	EXPECT_FALSE(requestedFileInfo.getIsCgi());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexAllowed());
	EXPECT_TRUE(requestedFileInfo.getIsAutoIndexFile());

	EXPECT_EQ("html", requestedFileInfo.getFileExtensionWithoutDot());
}

TEST_F(RequestedFileInfoTests, detectIndexHtml_AutoIndex)
{
	RequestedFileInfo requestedFileInfo({"test", "path", "www"}, false, this->getRouteConfigIndexEnabled(), this->logger);

	EXPECT_EQ("index.html", requestedFileInfo.getFileName());
	EXPECT_EQ("/test/path/www/index.html", requestedFileInfo.getCgiScriptName());
	EXPECT_EQ("", requestedFileInfo.getCgiPathInfo());
	EXPECT_EQ("", requestedFileInfo.getCgiPathTranslated());
	EXPECT_EQ("/www/index.html", requestedFileInfo.getTargetFilePathWithoutDocumentRoot());
	EXPECT_EQ(this->getTestFileDir() + "/www/index.html", requestedFileInfo.getTargetFilePath());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getDocumentRoot());

	EXPECT_FALSE(requestedFileInfo.getIsDirectory());
	EXPECT_FALSE(requestedFileInfo.getIsNotFound());
	EXPECT_FALSE(requestedFileInfo.getIsCgi());
	EXPECT_TRUE(requestedFileInfo.getIsAutoIndexAllowed());
	EXPECT_TRUE(requestedFileInfo.getIsAutoIndexFile());

	EXPECT_EQ("html", requestedFileInfo.getFileExtensionWithoutDot());
}

TEST_F(RequestedFileInfoTests, detectIndexPhp_NoPhpRoute)
{
	RequestedFileInfo requestedFileInfo({"test", "path", "php-cgi"}, false, this->getRouteConfig(), this->logger);

	EXPECT_EQ("index.php", requestedFileInfo.getFileName());
	EXPECT_EQ("/test/path/php-cgi/index.php", requestedFileInfo.getCgiScriptName());
	EXPECT_EQ("", requestedFileInfo.getCgiPathInfo());
	EXPECT_EQ("", requestedFileInfo.getCgiPathTranslated());
	EXPECT_EQ("/php-cgi/index.php", requestedFileInfo.getTargetFilePathWithoutDocumentRoot());
	EXPECT_EQ(this->getTestFileDir() + "/php-cgi/index.php", requestedFileInfo.getTargetFilePath());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getDocumentRoot());

	EXPECT_FALSE(requestedFileInfo.getIsDirectory());
	EXPECT_FALSE(requestedFileInfo.getIsNotFound());
	EXPECT_FALSE(requestedFileInfo.getIsCgi());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexAllowed());
	EXPECT_TRUE(requestedFileInfo.getIsAutoIndexFile());

	EXPECT_EQ("php", requestedFileInfo.getFileExtensionWithoutDot());
}

TEST_F(RequestedFileInfoTests, detectIndexPhp_PhpRoute)
{
	RequestedFileInfo requestedFileInfo({"test", "path", "php-cgi"}, false, this->getRouteConfigCgi(), this->logger);

	EXPECT_EQ("index.php", requestedFileInfo.getFileName());
	EXPECT_EQ("/test/path/php-cgi/index.php", requestedFileInfo.getCgiScriptName());
	EXPECT_EQ("", requestedFileInfo.getCgiPathInfo());
	EXPECT_EQ("", requestedFileInfo.getCgiPathTranslated());
	EXPECT_EQ("/php-cgi/index.php", requestedFileInfo.getTargetFilePathWithoutDocumentRoot());
	EXPECT_EQ(this->getTestFileDir() + "/php-cgi/index.php", requestedFileInfo.getTargetFilePath());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getDocumentRoot());

	EXPECT_FALSE(requestedFileInfo.getIsDirectory());
	EXPECT_FALSE(requestedFileInfo.getIsNotFound());
	EXPECT_TRUE(requestedFileInfo.getIsCgi());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexAllowed());
	EXPECT_TRUE(requestedFileInfo.getIsAutoIndexFile());

	EXPECT_EQ("php", requestedFileInfo.getFileExtensionWithoutDot());
	EXPECT_EQ(this->getRouteConfigCgi().getCgiConfigList()[0].getCgiExecutableFullPath(), requestedFileInfo.getCgiConfig().getCgiExecutableFullPath());
}
TEST_F(RequestedFileInfoTests, indexPhp_NoPhpRoute_PathInfo)
{
	RequestedFileInfo requestedFileInfo({"test", "path", "php-cgi", "index.php", "abc", "info"}, false, this->getRouteConfig(), this->logger);

	EXPECT_EQ("index.php", requestedFileInfo.getFileName());
	EXPECT_EQ("/test/path/php-cgi/index.php", requestedFileInfo.getCgiScriptName());
	EXPECT_EQ("/abc/info", requestedFileInfo.getCgiPathInfo());
	EXPECT_EQ(this->getTestFileDir() + "/abc/info", requestedFileInfo.getCgiPathTranslated());
	EXPECT_EQ("/php-cgi/index.php", requestedFileInfo.getTargetFilePathWithoutDocumentRoot());
	EXPECT_EQ(this->getTestFileDir() + "/php-cgi/index.php", requestedFileInfo.getTargetFilePath());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getDocumentRoot());

	EXPECT_FALSE(requestedFileInfo.getIsDirectory());
	EXPECT_TRUE(requestedFileInfo.getIsNotFound());
	EXPECT_FALSE(requestedFileInfo.getIsCgi());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexAllowed());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexFile());

	EXPECT_EQ("php", requestedFileInfo.getFileExtensionWithoutDot());
}

TEST_F(RequestedFileInfoTests, indexPhp_PhpRoute_PathInfo)
{
	RequestedFileInfo requestedFileInfo({"test", "path", "php-cgi", "index.php", "abc", "info"}, false, this->getRouteConfigCgi(), this->logger);

	EXPECT_EQ("index.php", requestedFileInfo.getFileName());
	EXPECT_EQ("/test/path/php-cgi/index.php", requestedFileInfo.getCgiScriptName());
	EXPECT_EQ("/abc/info", requestedFileInfo.getCgiPathInfo());
	EXPECT_EQ(this->getTestFileDir() + "/abc/info", requestedFileInfo.getCgiPathTranslated());
	EXPECT_EQ("/php-cgi/index.php", requestedFileInfo.getTargetFilePathWithoutDocumentRoot());
	EXPECT_EQ(this->getTestFileDir() + "/php-cgi/index.php", requestedFileInfo.getTargetFilePath());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getDocumentRoot());

	EXPECT_FALSE(requestedFileInfo.getIsDirectory());
	EXPECT_FALSE(requestedFileInfo.getIsNotFound());
	EXPECT_TRUE(requestedFileInfo.getIsCgi());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexAllowed());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexFile());

	EXPECT_EQ("php", requestedFileInfo.getFileExtensionWithoutDot());
	EXPECT_EQ(this->getRouteConfigCgi().getCgiConfigList()[0].getCgiExecutableFullPath(), requestedFileInfo.getCgiConfig().getCgiExecutableFullPath());
}

TEST_F(RequestedFileInfoTests, directoryRequestButWasFile)
{
	RequestedFileInfo requestedFileInfo({"test", "path", "www", "index.html"}, true, this->getRouteConfigCgi(), this->logger);

	EXPECT_EQ("index.html", requestedFileInfo.getFileName());
	EXPECT_EQ("/test/path/www/index.html", requestedFileInfo.getCgiScriptName());
	EXPECT_EQ("/", requestedFileInfo.getCgiPathInfo());
	EXPECT_EQ(this->getTestFileDir() + "/", requestedFileInfo.getCgiPathTranslated());
	EXPECT_EQ("/www/index.html", requestedFileInfo.getTargetFilePathWithoutDocumentRoot());
	EXPECT_EQ(this->getTestFileDir() + "/www/index.html", requestedFileInfo.getTargetFilePath());
	EXPECT_EQ(this->getTestFileDir(), requestedFileInfo.getDocumentRoot());

	EXPECT_FALSE(requestedFileInfo.getIsDirectory());
	EXPECT_TRUE(requestedFileInfo.getIsNotFound());
	EXPECT_FALSE(requestedFileInfo.getIsCgi());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexAllowed());
	EXPECT_FALSE(requestedFileInfo.getIsAutoIndexFile());
}

};	// namespace webserv
