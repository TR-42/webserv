#include <utils/splitNameValue.hpp>

#include "gtest/gtest.h"

TEST(splitNameValue, EmptyString)
{
	std::vector<uint8_t> data;
	std::pair<std::string, std::string> result = webserv::utils::splitNameValue(data);
	EXPECT_EQ(result.first, "");
	EXPECT_EQ(result.second, "");
}

TEST(splitNameValue, NoDelim)
{
	std::string inputStr = "abc";
	std::vector<uint8_t> data(inputStr.begin(), inputStr.end());
	std::pair<std::string, std::string> result = webserv::utils::splitNameValue(data);
	EXPECT_EQ(result.first, "");
	EXPECT_EQ(result.second, "");
}

TEST(splitNameValue, DelimAtEnd)
{
	std::string inputStr = "abc: ";
	std::vector<uint8_t> data(inputStr.begin(), inputStr.end());
	std::pair<std::string, std::string> result = webserv::utils::splitNameValue(data);
	EXPECT_EQ(result.first, "abc");
	EXPECT_EQ(result.second, "");
}

TEST(splitNameValue, DelimAtStart)
{
	std::string inputStr = ": abc";
	std::vector<uint8_t> data(inputStr.begin(), inputStr.end());
	std::pair<std::string, std::string> result = webserv::utils::splitNameValue(data);
	EXPECT_EQ(result.first, "");
	EXPECT_EQ(result.second, "");
}

TEST(splitNameValue, DelimInMiddle)
{
	std::string inputStr = "abc: def";
	std::vector<uint8_t> data(inputStr.begin(), inputStr.end());
	std::pair<std::string, std::string> result = webserv::utils::splitNameValue(data);
	EXPECT_EQ(result.first, "abc");
	EXPECT_EQ(result.second, "def");
}

TEST(splitNameValue, DelimInMiddleWithSpaces)
{
	std::string inputStr = "abc : def";
	std::vector<uint8_t> data(inputStr.begin(), inputStr.end());
	std::pair<std::string, std::string> result = webserv::utils::splitNameValue(data);
	EXPECT_EQ(result.first, "");
	EXPECT_EQ(result.second, "");
}

TEST(splitNameValue, DelimInMiddleWithSpacesAndTabs)
{
	std::string inputStr = "abc  \t:  def  \t";
	std::vector<uint8_t> data(inputStr.begin(), inputStr.end());
	std::pair<std::string, std::string> result = webserv::utils::splitNameValue(data);
	EXPECT_EQ(result.first, "");
	EXPECT_EQ(result.second, "");
}

TEST(splitNameValue, ValueWithSpace)
{
	std::string inputStr = "abc: def ghi";
	std::vector<uint8_t> data(inputStr.begin(), inputStr.end());
	std::pair<std::string, std::string> result = webserv::utils::splitNameValue(data);
	EXPECT_EQ(result.first, "abc");
	EXPECT_EQ(result.second, "def ghi");
}
