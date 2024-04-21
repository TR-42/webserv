#include <gtest/gtest.h>

#include <utils/UUID.hpp>

TEST(UUID, empty)
{
	std::string uuid = "00000000-0000-0000-0000-000000000000";
	EXPECT_EQ(webserv::utils::UUID(uuid).toString(), uuid);
};

TEST(UUID, empty_short)
{
	std::string uuidShort = "00000000000000000000000000000000";
	std::string uuid = "00000000-0000-0000-0000-000000000000";
	EXPECT_EQ(webserv::utils::UUID(uuidShort).toString(), uuid);
};
