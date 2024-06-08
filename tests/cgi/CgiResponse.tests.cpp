#include "cgi/CgiResponse.hpp"

#include "gtest/gtest.h"
#include "http/HttpResponse.hpp"
#include "utils/getTimeStr.hpp"

extern time_t __webserv_timeMockValue;

namespace webserv
{

static webserv::Logger logger;

TEST(CgiResponseTest, GenerateResponsePacket_DefaultValue)
{
	CgiResponse response(logger, utils::ErrorPageProvider());

	std::vector<uint8_t>
		expected = utils::ErrorPageProvider().internalServerError().generateResponsePacket(true, false);
	std::string expectedStr(expected.begin(), expected.end());
	std::vector<uint8_t> actual = response.generateResponsePacket(true);
	std::string actualStr(actual.begin(), actual.end());

	EXPECT_EQ(actualStr, expectedStr);
}

TEST(CgiResponseTest, Getters_DefaultValue)
{
	CgiResponse response(logger, utils::ErrorPageProvider());

	EXPECT_EQ(response.getMode(), CgiResponseMode::DOCUMENT);
	EXPECT_EQ(response.getContentType(), "");
	EXPECT_EQ(response.getLocation(), "");
	EXPECT_EQ(response.getStatusCode(), "200");
	EXPECT_EQ(response.getReasonPhrase(), "OK");
	EXPECT_EQ(response.getProtocolFieldMap(), HttpFieldMap());
	EXPECT_EQ(response.getExtensionFieldMap(), HttpFieldMap());
	EXPECT_EQ(response.getResponseBody().getBody(), std::vector<uint8_t>());
}

TEST(CgiResponseTest, Parse_Getter_DocumentResponse)
{
	CgiResponse cgiResponse(logger, utils::ErrorPageProvider());
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
	EXPECT_EQ(cgiResponse.getLocation(), "");
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
	std::vector<uint8_t> responseBody = cgiResponse.getResponseBody().getBody();
	std::string responseBodyStr(responseBody.begin(), responseBody.end());
	EXPECT_EQ(responseBodyStr, bodyStr);
}

TEST(CgiResponseTest, Parse_GenerateResponsePacket_DocumentResponse)
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

	std::vector<uint8_t> expected = httpResponse.generateResponsePacket(true, false);
	std::string expectedStr(expected.begin(), expected.end());

	CgiResponse cgiResponse(logger, utils::ErrorPageProvider());
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
	std::vector<uint8_t> actual = cgiResponse.generateResponsePacket(true);
	std::string actualStr(actual.begin(), actual.end());

	EXPECT_EQ(actualStr, expectedStr);
}

TEST(CgiResponseTest, Parse_Getter_StatusField)
{
	CgiResponse response(logger, utils::ErrorPageProvider());

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
	EXPECT_EQ(response.getLocation(), "");
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
	std::vector<uint8_t> responseBody = response.getResponseBody().getBody();
	std::string responseBodyStr(responseBody.begin(), responseBody.end());
	EXPECT_EQ(responseBodyStr, "No input file specified.");
}

TEST(CgiResponseTest, Parse_GenerateResponsePacket_StatusField)
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

	std::vector<uint8_t> expected = httpResponse.generateResponsePacket(true, false);
	std::string expectedStr(expected.begin(), expected.end());

	CgiResponse cgiResponse(logger, utils::ErrorPageProvider());
	std::string cgiResponseStr =
		"Status: 404 Not Found\n"
		"X-Powered-By: PHP/8.3.4\n"
		"Content-type: text/html; charset=UTF-8\n"
		"Date: Wed, 15 May 2024 12:34:56 GMT\n"
		"\n"
		"No input file specified.";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	cgiResponse.pushResponseRaw(cgiResponseVector);
	std::vector<uint8_t> actual = cgiResponse.generateResponsePacket(true);
	std::string actualStr(actual.begin(), actual.end());

	EXPECT_EQ(actualStr, expectedStr);
}

TEST(CgiResponseTest, Parse_Getter_NoBody)
{
	CgiResponse response(logger, utils::ErrorPageProvider());

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
	EXPECT_EQ(response.getLocation(), "");
	EXPECT_EQ(response.getStatusCode(), "501");
	EXPECT_EQ(response.getReasonPhrase(), "Not Implemented");
	EXPECT_EQ(response.getProtocolFieldMap().isNameExists("X-Powered-By"), true);
	if (response.getProtocolFieldMap().isNameExists("X-Powered-By")) {
		EXPECT_EQ(response.getProtocolFieldMap().getValueList("X-Powered-By")[0], "PHP/8.3.4 abc");
	}
	EXPECT_EQ(response.getProtocolFieldMap().isNameExists("date"), true);
	if (response.getProtocolFieldMap().isNameExists("date")) {
		EXPECT_EQ(response.getProtocolFieldMap().getValueList("date")[0], "Wed, 15 May 2024 12 : 34 : 56 GMT");
	}

	EXPECT_EQ(response.getExtensionFieldMap().isNameExists("x-ghi-abc"), false);
	EXPECT_EQ(response.getProtocolFieldMap().isNameExists("x-ghi-abc"), true);

	if (response.getProtocolFieldMap().isNameExists("x-ghi-abc")) {
		EXPECT_EQ(response.getProtocolFieldMap().getValueList("x-ghi-abc")[0], "def ghi");
	}
	std::vector<uint8_t> responseBody = response.getResponseBody().getBody();
	std::string responseBodyStr(responseBody.begin(), responseBody.end());
	EXPECT_EQ(responseBodyStr, "");
}

TEST(CgiResponseTest, Parse_GenerateResponsePacket_NoBody)
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

	std::vector<uint8_t> expected = httpResponse.generateResponsePacket(true, false);
	std::string expectedStr(expected.begin(), expected.end());

	CgiResponse cgiResponse(logger, utils::ErrorPageProvider());
	std::string cgiResponseStr =
		"status: 501 Not Implemented\n"
		"x-powered-by: PHP/8.3.4 abc\n"
		"content-type: text/html; charset=UTF-8\n"
		"date: Wed, 15 May 2024 12:34:56 GMT\n"
		"x-ghi-abc: def ghi\n"
		"\n";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	cgiResponse.pushResponseRaw(cgiResponseVector);
	std::vector<uint8_t> actual = cgiResponse.generateResponsePacket(true);
	std::string actualStr(actual.begin(), actual.end());

	EXPECT_EQ(actualStr, expectedStr);
}

TEST(CgiResponseTest, ResponseModeDocument)
{
	CgiResponse response(logger, utils::ErrorPageProvider());

	std::string cgiResponseStr =
		"Status: 200 OK\n"
		"Content-type: text/html; charset=UTF-8\n"
		"\n";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	EXPECT_TRUE(response.pushResponseRaw(cgiResponseVector));
	EXPECT_EQ(response.getMode(), CgiResponseMode::DOCUMENT);
	EXPECT_EQ(response.getContentType(), "text/html; charset=UTF-8");
	EXPECT_EQ(response.getStatusCode(), "200");
	EXPECT_EQ(response.getReasonPhrase(), "OK");
}

TEST(CgiResponseTest, ResponseModeLocalRedirect)
{
	CgiResponse response(logger, utils::ErrorPageProvider());

	std::string cgiResponseStr =
		"Location: /index.html\n"
		"\n";
	std::vector<uint8_t> cgiResponseVector2(cgiResponseStr.begin(), cgiResponseStr.end());
	EXPECT_TRUE(response.pushResponseRaw(cgiResponseVector2));
	EXPECT_EQ(response.getMode(), CgiResponseMode::LOCAL_REDIRECT);
	EXPECT_EQ(response.getLocation(), "/index.html");
}

TEST(CgiResponseTest, ResponseModeClientRedirect)
{
	__webserv_timeMockValue = std::time(NULL);
	CgiResponse response(logger, utils::ErrorPageProvider());

	std::string cgiResponseStr =
		"Location: http://localhost/index.html\n"
		"\n";
	std::vector<uint8_t> cgiResponseVector3(cgiResponseStr.begin(), cgiResponseStr.end());
	EXPECT_TRUE(response.pushResponseRaw(cgiResponseVector3));
	EXPECT_EQ(response.getMode(), CgiResponseMode::CLIENT_REDIRECT);
	EXPECT_EQ(response.getLocation(), "http://localhost/index.html");
	EXPECT_EQ(response.getResponseBody().size(), 0);

	std::vector<uint8_t> actual = response.generateResponsePacket(true);
	std::string timeStr = webserv::utils::getHttpTimeStr();
	std::string httpStr =
		"HTTP/1.1 301 Moved Permanently\r\n"
		"Location: http://localhost/index.html\r\n"
		"Content-Length: 0\r\n"
		"Date: " +
		timeStr +
		"\r\n"
		"Connection: close\r\n"
		"\r\n";
	std::string actualStr(actual.begin(), actual.end());
	EXPECT_EQ(actualStr, httpStr);
}

TEST(CgiResponseTest, ResponseModeClientRedirectWithBody)
{
	__webserv_timeMockValue = std::time(NULL);
	CgiResponse response(logger, utils::ErrorPageProvider());

	std::string cgiResponseStr =
		"Location: http://localhost/index.html\n"
		"\n"
		"abc";
	std::vector<uint8_t> cgiResponseVector4(cgiResponseStr.begin(), cgiResponseStr.end());
	EXPECT_FALSE(response.pushResponseRaw(cgiResponseVector4));
	EXPECT_EQ(response.getMode(), CgiResponseMode::CLIENT_REDIRECT);
	EXPECT_EQ(response.getLocation(), "http://localhost/index.html");
	EXPECT_EQ(response.getStatusCode(), "301");
	EXPECT_EQ(response.getReasonPhrase(), "Moved Permanently");
}

TEST(CgiResponseTest, ResponseModeClientRedirectWithDocument)
{
	__webserv_timeMockValue = std::time(NULL);
	CgiResponse response(logger, utils::ErrorPageProvider());
	std::string cgiResponseStr =
		"Status: 301 Moved Permanently\n"
		"Location: http://localhost/index.html\n"
		"Content-type: text/html; charset=UTF-8\n"
		"\n";
	std::vector<uint8_t> cgiResponseVector4(cgiResponseStr.begin(), cgiResponseStr.end());
	EXPECT_TRUE(response.pushResponseRaw(cgiResponseVector4));
	EXPECT_EQ(response.getMode(), CgiResponseMode::CLIENT_REDIRECT_WITH_DOCUMENT);
	EXPECT_EQ(response.getLocation(), "http://localhost/index.html");
	EXPECT_EQ(response.getStatusCode(), "301");
	EXPECT_EQ(response.getContentType(), "text/html; charset=UTF-8");
	EXPECT_EQ(response.getReasonPhrase(), "Moved Permanently");

	std::vector<uint8_t> actual = response.generateResponsePacket(true);
	std::string timeStr = webserv::utils::getHttpTimeStr();
	std::string httpStr =
		"HTTP/1.1 301 Moved Permanently\r\n"
		"Location: http://localhost/index.html\r\n"
		"Content-Type: text/html; charset=UTF-8\r\n"
		"Content-Length: 0\r\n"
		"Date: " +
		timeStr +
		"\r\n" +
		"Connection: close\r\n"
		"\r\n";

	std::string actualStr(actual.begin(), actual.end());
	EXPECT_EQ(actualStr, httpStr);
}

// 同じ名前のフィールドが複数ある場合の処理
TEST(CgiResponseTest, MultipleFields)
{
	CgiResponse response(logger, utils::ErrorPageProvider());

	std::string cgiResponseStr =
		"X-Powered-By: PHP/8.3.4\n"
		"X-Powered-By: PHP/8.3.5\n"
		"X-Powered-By: PHP/8.3.6\n"
		"Content-type: text/html; charset=UTF-8\n"
		"Date: Wed, 15 May 2024 12:34:56 GMT\n"
		"\n";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	EXPECT_TRUE(response.pushResponseRaw(cgiResponseVector));
	EXPECT_TRUE(response.getProtocolFieldMap().isNameExists("X-Powered-By"));
	if (response.getProtocolFieldMap().isNameExists("X-Powered-By")) {
		EXPECT_EQ(response.getProtocolFieldMap().getValueList("X-Powered-By").size(), 3);
		if (0 < response.getProtocolFieldMap().getValueList("X-Powered-By").size())
			EXPECT_EQ(response.getProtocolFieldMap().getValueList("X-Powered-By")[0], "PHP/8.3.4");
		if (1 < response.getProtocolFieldMap().getValueList("X-Powered-By").size())
			EXPECT_EQ(response.getProtocolFieldMap().getValueList("X-Powered-By")[1], "PHP/8.3.5");
		if (2 < response.getProtocolFieldMap().getValueList("X-Powered-By").size())
			EXPECT_EQ(response.getProtocolFieldMap().getValueList("X-Powered-By")[2], "PHP/8.3.6");
	}
}

TEST(CgiResponseTest, SetCookie)
{
	CgiResponse response(logger, utils::ErrorPageProvider());

	std::string cgiResponseStr =
		"Set-Cookie: PHP/8.3.4\n"
		"Set-Cookie: PHP/8.3.5\n"
		"Set-Cookie: PHP/8.3.6\n"
		"Content-type: text/html; charset=UTF-8\n"
		"Date: Wed, 15 May 2024 12:34:56 GMT\n"
		"\n";
	std::vector<uint8_t> cgiResponseVector(cgiResponseStr.begin(), cgiResponseStr.end());
	EXPECT_TRUE(response.pushResponseRaw(cgiResponseVector));
	EXPECT_TRUE(response.getProtocolFieldMap().isNameExists("Set-Cookie"));
	if (response.getProtocolFieldMap().isNameExists("Set-Cookie")) {
		EXPECT_EQ(response.getProtocolFieldMap().getValueList("Set-Cookie").size(), 3);
		if (0 < response.getProtocolFieldMap().getValueList("Set-Cookie").size())
			EXPECT_EQ(response.getProtocolFieldMap().getValueList("Set-Cookie")[0], "PHP/8.3.4");
		if (1 < response.getProtocolFieldMap().getValueList("Set-Cookie").size())
			EXPECT_EQ(response.getProtocolFieldMap().getValueList("Set-Cookie")[1], "PHP/8.3.5");
		if (2 < response.getProtocolFieldMap().getValueList("Set-Cookie").size())
			EXPECT_EQ(response.getProtocolFieldMap().getValueList("Set-Cookie")[2], "PHP/8.3.6");
	}
}

}	 // namespace webserv
