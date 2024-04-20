#pragma once

#include <Logger.hpp>
#include <map>
#include <string>
#include <types.hpp>
#include <vector>

#include "http/HttpFieldMap.hpp"

namespace webserv
{

class HttpRequest
{
 private:
	std::string _Method;
	std::string _Path;
	std::string _Version;
	HttpFieldMap _Headers;
	std::vector<uint8_t> _Body;

	bool _IsRequestLineParsed;
	bool _IsRequestHeaderParsed;
	bool _IsContentLengthHeaderParsed;
	size_t _ContentLength;

	std::vector<uint8_t> _UnparsedRequestRaw;
	// chunkは後で実装

 public:
	HttpRequest();
	HttpRequest(const std::string &requestText) : requestText(requestText) {}
	/**
	 * @brief 今までに受け取ったパケットとともに、HTTPリクエストを解析する
	 *
	 * @param requestRaw 受け取ったパケット
	 * @return true 特に問題なく処理が終了した
	 * @return false リクエストにエラーがあった
	 */
	bool pushRequestRaw(
		const std::vector<uint8_t> &requestRaw
	);

	const std::string &getMethod() const;
	const std::string &getPath() const;
	const std::string &getVersion() const;
	const HttpFieldMap &getHeaders() const;
	const std::vector<uint8_t> &getBody() const;
	bool isRequestLineParsed() const;
	bool isRequestHeaderParsed() const;
	size_t getContentLength();
	bool isRequestBodyLengthEnough();
	bool isRequestBodyLengthTooMuch();

 private:
	bool parseRequestLine(
		const std::vector<uint8_t> &requestRawLine
	);

	bool parseRequestHeader(
		const std::vector<uint8_t> &requestRawLine
	);
	Logger logger;
	std::string requestText;
};

}	 // namespace webserv
