#pragma once

#include <Logger.hpp>
#include <map>
#include <string>
#include <types.hpp>
#include <vector>

namespace webserv
{

typedef std::map<std::string, std::vector<std::string> > RequestHeaderMap;

class HttpRequest
{
 private:
	std::string _Method;
	std::string _Path;
	std::string _Version;
	RequestHeaderMap _Headers;
	std::vector<uint8_t> _Body;

	bool _IsRequestLineParsed;
	bool _IsRequestHeaderParsed;
	bool _IsContentLengthHeaderParsed;
	size_t _ContentLength;

	std::vector<uint8_t> _UnparsedRequestRaw;
	// chunkは後で実装

 public:
	HttpRequest();
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
	const RequestHeaderMap &getHeaders() const;
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
};

}	 // namespace webserv
