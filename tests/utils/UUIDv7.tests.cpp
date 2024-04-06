#include <gtest/gtest.h>

#include <utils/UUIDv7.hpp>

TEST(UUIDv7, basic)
{
	std::string uuid = "018eb3c9-6644-70a1-8a4a-ca36b2eb95f9";
	webserv::utils::UUIDv7 uuidValue(uuid);
	EXPECT_EQ(uuidValue.toString(), uuid);
	EXPECT_EQ(uuidValue.getTimestamp(), 1712413304388);
};

TEST(UUIDv7, empty)
{
	std::string uuid = "00000000-0000-7000-8000-000000000000";
	webserv::utils::UUIDv7 uuidValue(uuid);
	EXPECT_EQ(uuidValue.toString(), uuid);
	EXPECT_EQ(uuidValue.getTimestamp(), 0);
};
