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
	std::string expectedStr(expected.begin(), expected.end());
	std::vector<uint8_t> actual = response.generateResponsePacket();
	std::string actualStr(actual.begin(), actual.end());

	EXPECT_EQ(actualStr, expectedStr);
}

// getterのテスト
TEST(CgiResponseTest, Getters)
{
	CgiResponse response((Logger()));
	EXPECT_EQ(response.getMode(), CgiResponseMode::DOCUMENT);
	EXPECT_EQ(response.getContentType(), "");
	EXPECT_EQ(response.getLocalLocation(), "");
	EXPECT_EQ(response.getClientLocation(), "");
	EXPECT_EQ(response.getStatusCode(), "200");
	EXPECT_EQ(response.getReasonPhrase(), "OK");
	EXPECT_EQ(response.getProtocolFieldMap(), HttpFieldMap());
	EXPECT_EQ(response.getExtensionFieldMap(), HttpFieldMap());
	EXPECT_EQ(response.getResponseBody(), std::vector<uint8_t>());
}

TEST(CgiResponseTest, Parse)
{
	Logger logger;
	CgiResponse cgiResponse(logger);
	std::string cgiResponseStr =
		"X-Powered-By: PHP/8.3.4\n"
		"X-CGI-SampleField: SampleValue\n"
		"Content-type: text/html; charset=UTF-8\n"
		"Date: Wed, 15 May 2024 12:34:56 GMT\n"
		"\n"
		"<!doctype html>\n"
		"<html>\n"
		"  <head>\n"
		"    <meta charset=\"UTF-8\">\n"
		"    <title>Test</title>\n"
		"  </head>\n"
		"  <body>\n"
		"    <p>Hello, world!</p>\n"
		"    <p>num1=10&num2=20</p>\n"
		"  </body>\n"
		"</html>\n";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	cgiResponse.pushResponseRaw(cgiResponseVector);

	EXPECT_EQ(cgiResponse.getMode(), CgiResponseMode::DOCUMENT);
	EXPECT_EQ(cgiResponse.getContentType(), "text/html; charset=UTF-8");
	EXPECT_EQ(cgiResponse.getLocalLocation(), "");
	EXPECT_EQ(cgiResponse.getClientLocation(), "");
	EXPECT_EQ(cgiResponse.getStatusCode(), "200");
	EXPECT_EQ(cgiResponse.getReasonPhrase(), "OK");
	EXPECT_EQ(cgiResponse.getProtocolFieldMap().isNameExists("X-Powered-By"), true);
	if (cgiResponse.getProtocolFieldMap().isNameExists("X-Powered-By")) {
		EXPECT_EQ(cgiResponse.getProtocolFieldMap().getValueList("X-Powered-By")[0], "PHP/8.3.4");
	}
	EXPECT_EQ(cgiResponse.getProtocolFieldMap().isNameExists("X-CGI-SampleField"), false);
	EXPECT_EQ(cgiResponse.getProtocolFieldMap().isNameExists("Content-type"), false);
	EXPECT_EQ(cgiResponse.getProtocolFieldMap().isNameExists("Date"), true);
	if (cgiResponse.getProtocolFieldMap().isNameExists("Date")) {
		EXPECT_EQ(cgiResponse.getProtocolFieldMap().getValueList("Date")[0], "Wed, 15 May 2024 12:34:56 GMT");
	}

	std::string bodyStr =
		"<!doctype html>\n"
		"<html>\n"
		"  <head>\n"
		"    <meta charset=\"UTF-8\">\n"
		"    <title>Test</title>\n"
		"  </head>\n"
		"  <body>\n"
		"    <p>Hello, world!</p>\n"
		"    <p>num1=10&num2=20</p>\n"
		"  </body>\n"
		"</html>\n";
	std::vector<uint8_t> responseBody = cgiResponse.getResponseBody();
	std::string responseBodyStr(responseBody.begin(), responseBody.end());
	EXPECT_EQ(responseBodyStr, bodyStr);
}

// generateResponsePacket()メソッドのテスト
TEST(CgiResponseTest, GenerateExpectedResponsePacket)
{
	HttpResponse httpResponse;
	httpResponse.setVersion("HTTP/1.1");
	httpResponse.setStatusCode("200");
	httpResponse.setReasonPhrase("OK");
	HttpFieldMap headers;
	headers.addValue("X-Powered-By", "PHP/8.3.4");
	headers.addValue("X-CGI", "SampleValue");
	headers.addValue("X-CGI0", "SampleValue");
	headers.addValue("Content-Type", "text/html; charset=UTF-8");
	headers.addValue("Date", "Wed, 15 May 2024 12:34:56 GMT");
	httpResponse.setHeaders(headers);

	std::string bodyStr =
		"<!doctype html>\n"
		"<html>\n"
		"  <head>\n"
		"    <meta charset=\"UTF-8\">\n"
		"    <title>Test</title>\n"
		"  </head>\n"
		"  <body>\n"
		"    <p>Hello, world!</p>\n"
		"    <p>num1=10&num2=20</p>\n"
		"  </body>\n"
		"</html>\n";
	std::vector<uint8_t> body(bodyStr.begin(), bodyStr.end());
	httpResponse.setBody(body);

	std::vector<uint8_t> expected = httpResponse.generateResponsePacket();
	std::string expectedStr(expected.begin(), expected.end());

	Logger logger;
	CgiResponse cgiResponse(logger);
	std::string cgiResponseStr =
		"X-Powered-By: PHP/8.3.4\n"
		"X-CGI: SampleValue\n"
		"X-CGI0: SampleValue\n"
		"X-CGI-: SampleValue\n"
		"X-CGI-a: SampleValue\n"
		"Content-type: text/html; charset=UTF-8\n"
		"Date: Wed, 15 May 2024 12:34:56 GMT\n"
		"\n"
		"<!doctype html>\n"
		"<html>\n"
		"  <head>\n"
		"    <meta charset=\"UTF-8\">\n"
		"    <title>Test</title>\n"
		"  </head>\n"
		"  <body>\n"
		"    <p>Hello, world!</p>\n"
		"    <p>num1=10&num2=20</p>\n"
		"  </body>\n"
		"</html>\n";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	cgiResponse.pushResponseRaw(cgiResponseVector);
	std::vector<uint8_t> actual = cgiResponse.generateResponsePacket();
	std::string actualStr(actual.begin(), actual.end());

	EXPECT_EQ(actualStr, expectedStr);
}

// getterのテスト2
TEST(CgiResponseTest, Getters2)
{
	CgiResponse response((Logger()));
	std::string cgiResponseStr =
		"Status: 500 Internal Server Error\n"
		"X-Powered-By: PHP/8.3.4\n"
		"Content-type: text/html; charset=UTF-8\n"
		"Date: Wed, 15 May 2024 12:34:56 GMT\n"
		"\n"
		"No input file specified.";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	response.pushResponseRaw(cgiResponseVector);
	EXPECT_EQ(response.getMode(), CgiResponseMode::DOCUMENT);
	EXPECT_EQ(response.getContentType(), "text/html; charset=UTF-8");
	EXPECT_EQ(response.getLocalLocation(), "");
	EXPECT_EQ(response.getClientLocation(), "");
	EXPECT_EQ(response.getStatusCode(), "500");
	EXPECT_EQ(response.getReasonPhrase(), "Internal Server Error");
	EXPECT_EQ(response.getProtocolFieldMap().isNameExists("X-Powered-By"), true);
	if (response.getProtocolFieldMap().isNameExists("X-Powered-By")) {
		EXPECT_EQ(response.getProtocolFieldMap().getValueList("X-Powered-By")[0], "PHP/8.3.4");
	}
	EXPECT_EQ(response.getProtocolFieldMap().isNameExists("Content-type"), false);
	EXPECT_EQ(response.getProtocolFieldMap().isNameExists("Date"), true);
	if (response.getProtocolFieldMap().isNameExists("Date")) {
		EXPECT_EQ(response.getProtocolFieldMap().getValueList("Date")[0], "Wed, 15 May 2024 12:34:56 GMT");
	}
	std::vector<uint8_t> responseBody = response.getResponseBody();
	std::string responseBodyStr(responseBody.begin(), responseBody.end());
	EXPECT_EQ(responseBodyStr, "No input file specified.");
}

// generateResponsePacket()メソッドのテスト2
TEST(CgiResponseTest, GenerateExpectedResponsePacket2)
{
	HttpResponse httpResponse;
	httpResponse.setStatusCode("404");
	httpResponse.setReasonPhrase("Not Found");
	HttpFieldMap headers;
	headers.addValue("X-Powered-By", "PHP/8.3.4");
	headers.addValue("Content-Type", "text/html; charset=UTF-8");
	headers.addValue("Date", "Wed, 15 May 2024 12:34:56 GMT");
	httpResponse.setHeaders(headers);

	std::string bodyStr = "No input file specified.";

	std::vector<uint8_t>
		body(bodyStr.begin(), bodyStr.end());
	httpResponse.setBody(body);

	std::vector<uint8_t> expected = httpResponse.generateResponsePacket();
	std::string expectedStr(expected.begin(), expected.end());

	Logger logger;
	CgiResponse cgiResponse(logger);
	std::string cgiResponseStr =
		"Status: 404 Not Found\n"
		"X-Powered-By: PHP/8.3.4\n"
		"Content-type: text/html; charset=UTF-8\n"
		"Date: Wed, 15 May 2024 12:34:56 GMT\n"
		"\n"
		"No input file specified.";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	cgiResponse.pushResponseRaw(cgiResponseVector);
	std::vector<uint8_t> actual = cgiResponse.generateResponsePacket();
	std::string actualStr(actual.begin(), actual.end());

	EXPECT_EQ(actualStr, expectedStr);
}

// getterのテスト3
TEST(CgiResponseTest, Getters3)
{
	CgiResponse response((Logger()));
	std::string cgiResponseStr =
		"Status: 501 Not Implemented\n"
		"x-powered-by: PHP/8.3.4 abc\n"
		"date: Wed, 15 May 2024 12 : 34 : 56 GMT\n"
		"x-ghi-abc: def ghi\n"
		"\n";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	response.pushResponseRaw(cgiResponseVector);
	EXPECT_EQ(response.getMode(), CgiResponseMode::DOCUMENT);
	EXPECT_EQ(response.getContentType(), "");
	EXPECT_EQ(response.getLocalLocation(), "");
	EXPECT_EQ(response.getClientLocation(), "");
	EXPECT_EQ(response.getStatusCode(), "501");
	EXPECT_EQ(response.getReasonPhrase(), "Not Implemented");
	if (response.getProtocolFieldMap().isNameExists("X-Powered-By")) {
		EXPECT_EQ(response.getProtocolFieldMap().getValueList("X-Powered-By")[0], "PHP/8.3.4 abc");
	}
	if (response.getProtocolFieldMap().isNameExists("date")) {
		EXPECT_EQ(response.getProtocolFieldMap().getValueList("date")[0], "Wed, 15 May 2024 12 : 34 : 56 GMT");
	}

	EXPECT_EQ(response.getExtensionFieldMap().isNameExists("x-ghi-abc"), false);
	EXPECT_EQ(response.getProtocolFieldMap().isNameExists("x-ghi-abc"), true);

	if (response.getProtocolFieldMap().isNameExists("x-ghi-abc")) {
		EXPECT_EQ(response.getProtocolFieldMap().getValueList("x-ghi-abc")[0], "def ghi");
	}
	std::vector<uint8_t> responseBody = response.getResponseBody();
	std::string responseBodyStr(responseBody.begin(), responseBody.end());
	EXPECT_EQ(responseBodyStr, "");
}

// generateResponsePacket()メソッドのテスト3
TEST(CgiResponseTest, GenerateExpectedResponsePacket3)
{
	HttpResponse httpResponse;
	httpResponse.setVersion("HTTP/1.1");
	httpResponse.setStatusCode("501");
	httpResponse.setReasonPhrase("Not Implemented");
	HttpFieldMap headers;
	headers.addValue("X-Powered-By", "PHP/8.3.4 abc");
	headers.addValue("X-Ghi-Abc", "def ghi");
	headers.addValue("content-type", "text/html; charset=UTF-8");
	headers.addValue("Date", "Wed, 15 May 2024 12:34:56 GMT");
	httpResponse.setHeaders(headers);

	std::string bodyStr = "";
	std::vector<uint8_t> body(bodyStr.begin(), bodyStr.end());
	httpResponse.setBody(body);

	std::vector<uint8_t> expected = httpResponse.generateResponsePacket();
	std::string expectedStr(expected.begin(), expected.end());

	Logger logger;
	CgiResponse cgiResponse(logger);
	std::string cgiResponseStr =
		"status: 501 Not Implemented\n"
		"x-powered-by: PHP/8.3.4 abc\n"
		"content-type: text/html; charset=UTF-8\n"
		"date: Wed, 15 May 2024 12:34:56 GMT\n"
		"x-ghi-abc: def ghi\n";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	cgiResponse.pushResponseRaw(cgiResponseVector);
	std::vector<uint8_t> actual = cgiResponse.generateResponsePacket();
	std::string actualStr(actual.begin(), actual.end());

	EXPECT_EQ(actualStr, expectedStr);
}

}	 // namespace webserv
