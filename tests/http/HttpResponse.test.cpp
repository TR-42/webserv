#include <gtest/gtest.h>

#include <http/HttpResponse.hpp>

TEST(HttpResponse, test1)
{
	webserv::HttpResponse response;
	response.setVersion("HTTP/1.1");
	response.setStatusCode("200");
	response.setReasonPhrase("OK");
	webserv::ResponseHeaderMap headers;
	headers["Content-Type"] = std::vector<std::string>();
	headers["Content-Type"].push_back("text/html");
	response.setHeaders(headers);
	std::string body = "Hello, World!";
	std::vector<uint8_t> bodyVec(body.begin(), body.end());
	response.setBody(bodyVec);
	std::vector<uint8_t> responsePacket = response.generateResponsePacket();
	std::string responsePacketStr(responsePacket.begin(), responsePacket.end());
	std::string expectedResponsePacketStr = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nHello, World!";
	EXPECT_EQ(responsePacketStr, expectedResponsePacketStr);
}
