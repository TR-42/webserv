#include <gtest/gtest.h>

#include <http/HttpRequest.hpp>

#define REQ_LINE_CASE_1 "GET /index.html HTTP/1.1\r\n"
#define REQ_HEADER_CASE_1 REQ_LINE_CASE_1 "TestKey: TestValue\r\n"

#define REQ_HEADER_CASE_2 REQ_HEADER_CASE_1 "\r\n"
#define REQ_HEADER_CASE_3 REQ_HEADER_CASE_1 "TestKey2: TestValue2\r\n"
#define REQ_HEADER_CASE_4 REQ_HEADER_CASE_1 "TestKey: TestValue2\r\n"
#define REQ_HEADER_CASE_5 REQ_LINE_CASE_1 "TestKey:TestValue\r\n"
#define REQ_HEADER_CASE_6 REQ_LINE_CASE_1 "TestKey:  TestValue  \r\n"
#define CONTENT_LENGTH_CASE_1 REQ_LINE_CASE_1 "Content-Length: 10\r\n\r\n"

#define REQ_LINE_ERR_CASE_1 "GET  /index.html HTTP/1.1\r\n"
#define REQ_LINE_ERR_CASE_2 "GET /index.html  HTTP/1.1\r\n"
#define REQ_LINE_ERR_CASE_3 "GET\r\n"
#define REQ_LINE_ERR_CASE_4 "/index.html\r\n"
#define REQ_LINE_ERR_CASE_5 "HTTP/1.1\r\n"
#define REQ_LINE_ERR_CASE_6 "GET HTTP/1.1\r\n"
#define REQ_LINE_ERR_CASE_7 "GET /index.html\r\n"
#define REQ_LINE_ERR_CASE_8 "\r\n"
#define REQ_LINE_ERR_CASE_9 " GET /index.html HTTP/1.1\r\n"
#define REQ_LINE_ERR_CASE_10 "GET /index.html HTTP/1.1 \r\n"

TEST(HttpRequest, RequestLine)
{
	webserv::HttpRequest request;
	std::string reqLine = REQ_LINE_CASE_1;
	std::vector<uint8_t> reqPacket(reqLine.begin(), reqLine.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request.isRequestLineParsed(), true);
	EXPECT_EQ(request.getMethod(), "GET");
	EXPECT_EQ(request.getPath(), "/index.html");
	EXPECT_EQ(request.getVersion(), "HTTP/1.1");
}

TEST(HttpRequest, RequestHeader)
{
	webserv::HttpRequest request;
	std::string reqStr = REQ_HEADER_CASE_1;
	std::vector<uint8_t> reqPacket(reqStr.begin(), reqStr.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request.isRequestLineParsed(), true);
	EXPECT_EQ(request.getHeaders().empty(), false);
	EXPECT_EQ(request.getHeaders().at("TestKey")[0], "TestValue");
}

TEST(HttpRequest, RequestHeader2)
{
	webserv::HttpRequest request;
	std::string reqStr = REQ_HEADER_CASE_2;
	std::vector<uint8_t> reqPacket(reqStr.begin(), reqStr.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request.isRequestLineParsed(), true);
	EXPECT_EQ(request.isRequestHeaderParsed(), true);
}

TEST(HttpRequest, RequestHeader_MultiKey)
{
	webserv::HttpRequest request;
	std::string reqStr = REQ_HEADER_CASE_3;
	std::vector<uint8_t> reqPacket(reqStr.begin(), reqStr.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request.isRequestLineParsed(), true);
	EXPECT_EQ(request.getHeaders().empty(), false);
	EXPECT_EQ(request.getHeaders().at("TestKey")[0], "TestValue");
	EXPECT_EQ(request.getHeaders().at("TestKey2")[0], "TestValue2");
}

TEST(HttpRequest, RequestHeader_MultiValue)
{
	webserv::HttpRequest request;
	std::string reqStr = REQ_HEADER_CASE_4;
	std::vector<uint8_t> reqPacket(reqStr.begin(), reqStr.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request.isRequestLineParsed(), true);
	EXPECT_EQ(request.getHeaders().empty(), false);
	EXPECT_EQ(request.getHeaders().at("TestKey")[0], "TestValue");
	EXPECT_EQ(request.getHeaders().at("TestKey")[1], "TestValue2");
}

TEST(HttpRequest, RequestHeader5_NoSPC)
{
	webserv::HttpRequest request;
	std::string reqStr = REQ_HEADER_CASE_5;
	std::vector<uint8_t> reqPacket(reqStr.begin(), reqStr.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request.isRequestLineParsed(), true);
	EXPECT_EQ(request.getHeaders().at("TestKey")[0], "TestValue");
}

TEST(HttpRequest, RequestHeader6_MultiSPC)
{
	webserv::HttpRequest request;
	std::string reqStr = REQ_HEADER_CASE_6;
	std::vector<uint8_t> reqPacket(reqStr.begin(), reqStr.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request.isRequestLineParsed(), true);
	EXPECT_EQ(request.getHeaders().at("TestKey")[0], "TestValue");
}

TEST(HttpRequest, RequestHeader_ContentLength)
{
	webserv::HttpRequest request;
	std::string reqStr = CONTENT_LENGTH_CASE_1;
	std::vector<uint8_t> reqPacket(reqStr.begin(), reqStr.end());

	EXPECT_EQ(request.pushRequestRaw(reqPacket), true);
	EXPECT_EQ(request.isRequestLineParsed(), true);
	EXPECT_EQ(request.isRequestHeaderParsed(), true);
	EXPECT_EQ(request.getContentLength(), 10);
}

#define TEST_REQ_LINE_ERROR_CASE(CASE) \
	TEST(HttpRequest, RequestLineError_##CASE) \
	{ \
		webserv::HttpRequest request; \
		std::string reqLine = CASE; \
		std::vector<uint8_t> reqPacket(reqLine.begin(), reqLine.end()); \
		EXPECT_EQ(request.pushRequestRaw(reqPacket), false); \
		EXPECT_EQ(request.isRequestLineParsed(), false); \
	}
TEST_REQ_LINE_ERROR_CASE(REQ_LINE_ERR_CASE_1)
TEST_REQ_LINE_ERROR_CASE(REQ_LINE_ERR_CASE_2)
TEST_REQ_LINE_ERROR_CASE(REQ_LINE_ERR_CASE_3)
TEST_REQ_LINE_ERROR_CASE(REQ_LINE_ERR_CASE_4)
TEST_REQ_LINE_ERROR_CASE(REQ_LINE_ERR_CASE_5)
TEST_REQ_LINE_ERROR_CASE(REQ_LINE_ERR_CASE_6)
TEST_REQ_LINE_ERROR_CASE(REQ_LINE_ERR_CASE_7)
TEST_REQ_LINE_ERROR_CASE(REQ_LINE_ERR_CASE_8)
TEST_REQ_LINE_ERROR_CASE(REQ_LINE_ERR_CASE_9)
TEST_REQ_LINE_ERROR_CASE(REQ_LINE_ERR_CASE_10)
