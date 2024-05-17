#include <gtest/gtest.h>

#include <utils.hpp>

TEST(splitWithSpace, Empty)
{
	std::string dataStr = "";
	std::vector<std::string> result = webserv::utils::splitWithSpace(dataStr, 2);
	EXPECT_EQ(result.size(), 0);
}

TEST(splitWithSpace, OneWord)
{
	std::string dataStr = "abc";
	std::vector<std::string> result = webserv::utils::splitWithSpace(dataStr, 2);
	EXPECT_EQ(result.size(), 1);
	EXPECT_EQ(result[0], "abc");
}

TEST(splitWithSpace, TwoWords)
{
	std::string dataStr = "abc def";
	std::vector<std::string> result = webserv::utils::splitWithSpace(dataStr, 2);
	EXPECT_EQ(result.size(), 2);
	EXPECT_EQ(result[0], "abc");
	EXPECT_EQ(result[1], "def");
}

TEST(splitWithSpace, TwoWordsWithSpaces)
{
	std::string dataStr = "abc  def";
	std::vector<std::string> result = webserv::utils::splitWithSpace(dataStr, 2);
	EXPECT_EQ(result.size(), 2);
	EXPECT_EQ(result[0], "abc");
	EXPECT_EQ(result[1], "def");
}

TEST(splitWithSpace, TwoWordsWithTabs)
{
	std::string dataStr = "abc\tdef";
	std::vector<std::string> result = webserv::utils::splitWithSpace(dataStr, 2);
	EXPECT_EQ(result.size(), 1);
	EXPECT_EQ(result[0], "abc\tdef");
}

TEST(splitWithSpace, NotFound)
{
	std::string dataStr = "404 Not Found";
	std::vector<std::string> result = webserv::utils::splitWithSpace(dataStr, 2);
	EXPECT_EQ(result.size(), 2);
	EXPECT_EQ(result[0], "404");
	EXPECT_EQ(result[1], "Not Found");
}
