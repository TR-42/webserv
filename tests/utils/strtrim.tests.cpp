#include "gtest/gtest.h"
#include "utils.hpp"

TEST(strtrim, EmptyString)
{
	EXPECT_EQ(webserv::utils::strtrim(""), "");
}

TEST(strtrim, NoSpaces)
{
	EXPECT_EQ(webserv::utils::strtrim("abc"), "abc");
}

TEST(strtrim, LeadingSpaces)
{
	EXPECT_EQ(webserv::utils::strtrim("  abc"), "abc");
}

TEST(strtrim, TrailingSpaces)
{
	EXPECT_EQ(webserv::utils::strtrim("abc  "), "abc");
}

TEST(strtrim, LeadingAndTrailingSpaces)
{
	EXPECT_EQ(webserv::utils::strtrim("  abc  "), "abc");
}

TEST(strtrim, LeadingAndTrailingTabs)
{
	EXPECT_EQ(webserv::utils::strtrim("\tabc\t"), "abc");
}

TEST(strtrim, LeadingAndTrailingNewlines)
{
	EXPECT_EQ(webserv::utils::strtrim("\nabc\n"), "abc");
}

TEST(strtrim, LeadingAndTrailingSpacesTabsNewlines)
{
	EXPECT_EQ(webserv::utils::strtrim(" \t\nabc \t\n"), "abc");
}

TEST(strtrim, LeadingAndTrailingSpacesTabsNewlinesMixed)
{
	EXPECT_EQ(webserv::utils::strtrim(" \t\nabc \t\n"), "abc");
}

TEST(strtrim, LeadingAndTrailingSpacesTabsNewlinesMixed2)
{
	EXPECT_EQ(webserv::utils::strtrim(" \t\nabc \t\n "), "abc");
}

TEST(strtrim, LeadingAndTrailingSpacesTabsNewlinesMixed3)
{
	EXPECT_EQ(webserv::utils::strtrim(" \t\nabc \t\n  "), "abc");
}

TEST(strtrim, withSpace)
{
	EXPECT_EQ(webserv::utils::strtrim("Not Found"), "Not Found");
}
