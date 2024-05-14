#include "cgi/CgiResponse.hpp"

#include "gtest/gtest.h"
#include "http/HttpResponse.hpp"

namespace webserv
{

// generateResponsePacket()メソッドのテスト
TEST(CgiResponseTest, GenerateResponsePacket)
{
	HttpResponse httpResponse;
	httpResponse.setVersion("HTTP/1.1");
	httpResponse.setStatusCode("200");
	httpResponse.setReasonPhrase("OK");
	httpResponse.setHeaders(HttpFieldMap());
	httpResponse.setBody(std::vector<uint8_t>());

	CgiResponse response((Logger()));
	std::vector<uint8_t>
		expected = httpResponse.generateResponsePacket();
	std::vector<uint8_t> actual = response.generateResponsePacket();

	EXPECT_EQ(actual, expected);
}

// getterのテスト
TEST(CgiResponseTest, Getters)
{
	CgiResponse response((Logger()));
	EXPECT_EQ(response.getMode(), CgiResponseMode::DOCUMENT);
	EXPECT_EQ(response.getContentType(), "text/html");
	EXPECT_EQ(response.getLocalLocation(), "");
	EXPECT_EQ(response.getClientLocation(), "");
	EXPECT_EQ(response.getStatusCode(), "200");
	EXPECT_EQ(response.getReasonPhrase(), "OK");
	EXPECT_EQ(response.getProtocolFieldMap(), HttpFieldMap());
	EXPECT_EQ(response.getExtensionFieldMap(), HttpFieldMap());
	EXPECT_EQ(response.getResponseBody(), std::vector<uint8_t>());
}

}	 // namespace webserv
