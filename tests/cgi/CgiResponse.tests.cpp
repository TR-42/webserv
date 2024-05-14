#include <gtest/gtest.h>

#include <cgi/CgiResponse.hpp>
#include <http/HttpResponse.hpp>
#include <iostream>
#include <utils.hpp>

std::string removeDateHeader(const std::string &response)
{
	std::string result = response;
	std::string::size_type datePos = result.find("Date:");
	if (datePos != std::string::npos) {
		std::string::size_type endOfLine = result.find("\r\n", datePos);
		if (endOfLine != std::string::npos) {
			result.erase(datePos, endOfLine - datePos + 2);
		}
	}
	return result;
}

TEST(CgiResponse, testGenerateResponsePacket)
{
	webserv::CgiResponse cgiResponse;
	cgiResponse.setResponseBody("Hello, CGI World!");

	std::vector<uint8_t> responsePacket = cgiResponse.generateResponsePacket();

	std::string responsePacketStr(responsePacket.begin(), responsePacket.end());
	std::string expectedResponsePacketStr = "HTTP/1.1 200 CGI Response\r\nContent-Type: text/html\r\nContent-Length: 17\r\n\r\nHello, CGI World!";

	std::string actual = removeDateHeader(responsePacketStr);

	EXPECT_EQ(actual, expectedResponsePacketStr);
}

TEST(CgiResponse, testCustomStatusAndHeaders)
{
	webserv::CgiResponse cgiResponse;
	cgiResponse.setStatusCode("404");
	cgiResponse.setReasonPhrase("Not Found");
	cgiResponse.setContentType("text/plain");
	cgiResponse.setResponseBody("Not Found");

	std::vector<uint8_t> responsePacket = cgiResponse.generateResponsePacket();

	std::string responsePacketStr(responsePacket.begin(), responsePacket.end());
	std::string expectedResponsePacketStr = "HTTP/1.1 404 CGI Response\r\nContent-Type: text/plain\r\nContent-Length: 9\r\n\r\nNot Found";

	std::string actual = removeDateHeader(responsePacketStr);

	std::cerr << "Generated Response: " << actual << std::endl;
	std::cerr << "Expected Response: " << expectedResponsePacketStr << std::endl;

	EXPECT_EQ(actual, expectedResponsePacketStr);
}
