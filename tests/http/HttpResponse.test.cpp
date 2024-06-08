#include <gtest/gtest.h>

#include <http/HttpResponse.hpp>
#include <utils/getTimeStr.hpp>

#include "http/HttpFieldMap.hpp"

extern time_t __webserv_timeMockValue;

TEST(HttpResponse, test1)
{
	__webserv_timeMockValue = std::time(NULL);
	webserv::HttpResponse response;
	response.setVersion("HTTP/1.1");
	response.setStatusCode("200");
	response.setReasonPhrase("OK");
	webserv::HttpFieldMap headers;
	headers.addValue("Content-Type", "text/html");
	response.setHeaders(headers);
	std::string body = "Hello, World!";
	std::vector<uint8_t> bodyVec(body.begin(), body.end());
	response.setBody(bodyVec);
	std::vector<uint8_t> responsePacket = response.generateResponsePacket(true, false);
	std::string responsePacketStr(responsePacket.begin(), responsePacket.end());
	std::string expectedResponsePacketStr =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 13\r\n"
		"Date: " +
		webserv::utils::getHttpTimeStr() +
		"\r\n"
		"Connection: close\r\n"
		"\r\n"
		"Hello, World!";
	EXPECT_EQ(responsePacketStr, expectedResponsePacketStr);
}
