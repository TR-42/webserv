#include <gtest/gtest.h>

#include <http/HttpRequest.hpp>

#define REQ_LINE_CASE_1 "GET /index.html HTTP/1.1\r\n"

TEST(HttpRequest, RequestLine)
{
	webserv::HttpRequest request;
	std::vector<uint8_t> reqPacket;
	std::string reqLine = REQ_LINE_CASE_1;

	reqPacket.insert(reqPacket.end(), reqLine.begin(), reqLine.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request._IsRequestLineParsed, true);
	EXPECT_EQ(request._Method, "GET");
	EXPECT_EQ(request._Path, "/index.html");
	EXPECT_EQ(request._Version, "HTTP/1.1");
}
