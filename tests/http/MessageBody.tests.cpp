#include <gtest/gtest.h>

#include <http/MessageBody.hpp>

using namespace webserv;

TEST(MessageBody, noChunk0)
{
	MessageBody body(false, false, 0);

	EXPECT_FALSE(body.getIsChunked());
	EXPECT_EQ(body.getContentLength(), 0);
	EXPECT_TRUE(body.getIsProcessComplete());
	EXPECT_EQ(body.getBody().size(), 0);

	std::string data = "1234567890";
	EXPECT_TRUE(body.pushData((const uint8_t *)data.c_str(), 10));

	EXPECT_TRUE(body.getIsProcessComplete());
	EXPECT_EQ(body.getBody().size(), 0);
}

TEST(MessageBody, noChunk10)
{
	MessageBody body(false, false, 10);

	EXPECT_FALSE(body.getIsChunked());
	EXPECT_EQ(body.getContentLength(), 10);
	EXPECT_FALSE(body.getIsProcessComplete());
	EXPECT_EQ(body.getBody().size(), 0);

	std::string data = "1234567890";
	EXPECT_TRUE(body.pushData((const uint8_t *)data.c_str(), 10));

	EXPECT_TRUE(body.getIsProcessComplete());
	EXPECT_EQ(body.getBody().size(), 10);
	const std::vector<uint8_t> &actual = body.getBody();
	std::string actualStr(actual.begin(), actual.end());
	EXPECT_EQ(actualStr, data);
}

TEST(MessageBody, Chunked0)
{
	MessageBody body(false, true, 0);

	EXPECT_TRUE(body.getIsChunked());
	EXPECT_EQ(body.getContentLength(), 0);
	EXPECT_FALSE(body.getIsProcessComplete());
	EXPECT_EQ(body.getBody().size(), 0);

	std::string data =
		"0\r\n"
		"\r\n";
	EXPECT_TRUE(body.pushData((const uint8_t *)data.c_str(), data.length()));

	EXPECT_TRUE(body.getIsProcessComplete());
	EXPECT_EQ(body.getBody().size(), 0);
}

TEST(MessageBody, Chunked10)
{
	MessageBody body(false, true, 0);

	EXPECT_TRUE(body.getIsChunked());
	EXPECT_EQ(body.getContentLength(), 0);
	EXPECT_FALSE(body.getIsProcessComplete());
	EXPECT_EQ(body.getBody().size(), 0);

	std::string data =
		"a\r\n"
		"1234567890\r\n"
		"0\r\n"
		"\r\n";
	EXPECT_TRUE(body.pushData((const uint8_t *)data.c_str(), data.length()));

	EXPECT_TRUE(body.getIsProcessComplete());
	EXPECT_EQ(body.getBody().size(), 10);
	const std::vector<uint8_t> &actual = body.getBody();
	std::string actualStr(actual.begin(), actual.end());
	EXPECT_EQ(actualStr, "1234567890");
}
