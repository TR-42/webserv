#include <utils/stoul.hpp>

#include "gtest/gtest.h"

TEST(stoul, EmptyString)
{
	unsigned long result;
	EXPECT_FALSE(webserv::utils::stoul("", result));
}

TEST(stoul, InvalidString)
{
	unsigned long result;
	EXPECT_FALSE(webserv::utils::stoul("a", result));
	EXPECT_FALSE(webserv::utils::stoul("1a", result));
	EXPECT_FALSE(webserv::utils::stoul("a1", result));
	EXPECT_FALSE(webserv::utils::stoul("a1a", result));
	EXPECT_FALSE(webserv::utils::stoul(" 1", result));
	EXPECT_FALSE(webserv::utils::stoul("-1", result));
	EXPECT_FALSE(webserv::utils::stoul("+1", result));
}

TEST(stoul, Zero)
{
	unsigned long result;
	EXPECT_TRUE(webserv::utils::stoul("0", result));
	EXPECT_EQ(0, result);
}

TEST(stoul, One)
{
	unsigned long result;
	EXPECT_TRUE(webserv::utils::stoul("1", result));
	EXPECT_EQ(1, result);
}

TEST(stoul, More)
{
	unsigned long result;
	EXPECT_TRUE(webserv::utils::stoul("100", result));
	EXPECT_EQ(100, result);
}

TEST(stoul, LeadingZero)
{
	unsigned long result;
	EXPECT_TRUE(webserv::utils::stoul("0100", result));
	EXPECT_EQ(100, result);
}

TEST(stoul, MAX)
{
	unsigned long result;
	EXPECT_TRUE(webserv::utils::stoul("18446744073709551615", result));
	EXPECT_EQ(18446744073709551615UL, result);
}

TEST(stoul, UnsignedIntMaxValue)
{
	unsigned long result;
	EXPECT_TRUE(webserv::utils::stoul("4294967295", result));
	EXPECT_EQ(4294967295UL, result);
}

TEST(stoul, UnsignedIntMaxValuePlusOne)
{
	unsigned long result;
	EXPECT_TRUE(webserv::utils::stoul("4294967296", result));
	EXPECT_EQ(4294967296UL, result);
}

TEST(stoul, Overflow)
{
	unsigned long result;
	EXPECT_FALSE(webserv::utils::stoul("18446744073709551616", result));
	EXPECT_FALSE(webserv::utils::stoul("184467440737095516150", result));
}
