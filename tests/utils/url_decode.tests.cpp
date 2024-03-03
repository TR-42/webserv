#include <gtest/gtest.h>

#include <utils.hpp>

TEST(url_decode, empty)
{
	EXPECT_EQ("", webserv::utils::url_decode(""));
};

TEST(url_decode, no_escape)
{
	EXPECT_EQ("a", webserv::utils::url_decode("a"));
	EXPECT_EQ("1", webserv::utils::url_decode("1"));
	EXPECT_EQ("A", webserv::utils::url_decode("A"));
	EXPECT_EQ("&", webserv::utils::url_decode("&"));
};

TEST(url_decode, only_escape)
{
	EXPECT_EQ("\x01", webserv::utils::url_decode("%01"));
	EXPECT_EQ("\x11", webserv::utils::url_decode("%11"));
	EXPECT_EQ("\xAF", webserv::utils::url_decode("%AF"));
	EXPECT_EQ("\xFF\xAA", webserv::utils::url_decode("%FF%AA"));
};

TEST(url_decode, invalid_escape)
{
	EXPECT_THROW(webserv::utils::url_decode("%"), std::invalid_argument);
	EXPECT_THROW(webserv::utils::url_decode("%0"), std::invalid_argument);
	EXPECT_THROW(webserv::utils::url_decode("%0G"), std::invalid_argument);
	EXPECT_THROW(webserv::utils::url_decode("%0g"), std::invalid_argument);
	EXPECT_THROW(webserv::utils::url_decode("%G0"), std::invalid_argument);
	EXPECT_THROW(webserv::utils::url_decode("%g0"), std::invalid_argument);
	EXPECT_THROW(webserv::utils::url_decode("%aa%g0"), std::invalid_argument);
	EXPECT_THROW(webserv::utils::url_decode("%aa%g0X"), std::invalid_argument);
};
