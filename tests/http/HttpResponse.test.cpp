#include <gtest/gtest.h>

#include <http/HttpResponse.hpp>
#include <utils/getTimeStr.hpp>

#include "http/HttpFieldMap.hpp"

TEST(HttpResponse, test1)
{
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
	std::vector<uint8_t> responsePacket = response.generateResponsePacket(true);
	time_t time = std::time(NULL);
	std::string responsePacketStr(responsePacket.begin(), responsePacket.end());
	std::string timeStr = webserv::utils::getHttpTimeStr(time);
	std::string expectedResponsePacketStr = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 13\r\nDate: " + timeStr + "\r\n\r\nHello, World!";
	std::string timeStr1 = webserv::utils::getHttpTimeStr(time + 1);
	std::string expectedResponsePacketStr1 = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 13\r\nDate: " + timeStr1 + "\r\n\r\nHello, World!";
	// 1秒後までは許容する
	if (expectedResponsePacketStr1 == responsePacketStr) {
		EXPECT_EQ(responsePacketStr, expectedResponsePacketStr);
	} else {
		EXPECT_EQ(responsePacketStr, expectedResponsePacketStr);
	}
}
