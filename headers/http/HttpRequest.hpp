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
	const Logger &logger;
	std::string _Method;
	std::string _Path;
	std::string _Query;
	std::string _Version;
	HttpFieldMap _Headers;
	std::vector<uint8_t> _Body;

	bool _IsRequestLineParsed;
	bool _IsRequestHeaderParsed;
	bool _IsParseCompleted;
	size_t _ContentLength;
	std::string _Host;
	bool _IsChunkedRequest;
	std::string _NormalizedPath;

	std::vector<uint8_t> _UnparsedRequestRaw;
	// chunkは後で実装

 public:
	HttpRequest(const Logger &logger);
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
	const std::string &getQuery() const;
	const std::string &getVersion() const;
	const HttpFieldMap &getHeaders() const;
	const std::vector<uint8_t> &getBody() const;
	bool isRequestLineParsed() const;
	bool isRequestHeaderParsed() const;
	size_t getContentLength() const;
	bool isRequestBodyLengthEnough() const;
	bool isRequestBodyLengthTooMuch() const;
	bool isParseCompleted();
	std::string getHost() const;
	bool isChunkedRequest() const;
	std::string getNormalizedPath() const;

 private:
	bool parseRequestLine(
		const std::vector<uint8_t> &requestRawLine
	);

	bool parseRequestHeader(
		const std::vector<uint8_t> &requestRawLine
	);

	std::string requestText;
};

}	 // namespace webserv
