#include <gtest/gtest.h>

#include <http/HttpRequest.hpp>

#define REQ_LINE_CASE_1 "GET /index.html HTTP/1.1\r\n"
#define REQ_HEADER_CASE_1 REQ_LINE_CASE_1 "TestKey: TestValue\r\n"

#define REQ_HEADER_CASE_2 REQ_HEADER_CASE_1 "\r\n"

TEST(HttpRequest, RequestLine)
{
	webserv::HttpRequest request;
	std::string reqLine = REQ_LINE_CASE_1;
	std::vector<uint8_t> reqPacket(reqLine.begin(), reqLine.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request._IsRequestLineParsed, true);
	EXPECT_EQ(request._Method, "GET");
	EXPECT_EQ(request._Path, "/index.html");
	EXPECT_EQ(request._Version, "HTTP/1.1");
}

TEST(HttpRequest, RequestHeader)
{
	webserv::HttpRequest request;
	std::string reqStr = REQ_HEADER_CASE_1;
	std::vector<uint8_t> reqPacket(reqStr.begin(), reqStr.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request._IsRequestLineParsed, true);
	EXPECT_EQ(request._Headers.empty(), false);
	EXPECT_EQ(request._Headers["TestKey"][0], "TestValue");
}

TEST(HttpRequest, RequestHeader2)
{
	webserv::HttpRequest request;
	std::string reqStr = REQ_HEADER_CASE_2;
	std::vector<uint8_t> reqPacket(reqStr.begin(), reqStr.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request._IsRequestLineParsed, true);
	EXPECT_EQ(request._IsRequestHeaderParsed, true);
}
