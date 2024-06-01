#pragma once

#include <Logger.hpp>
#include <map>
#include <stdexcept>
#include <string>
#include <types.hpp>
#include <vector>

#include "./HttpVersion.hpp"
#include "config/ServerRunningConfig.hpp"
#include "http/HttpFieldMap.hpp"

namespace webserv
{

class HttpRequest
{
 private:
	HttpRequest &operator=(const HttpRequest &src)
	{
		(void)src;
		throw std::runtime_error("HttpRequest copy assignment operator is not allowed");
	}

	const Logger &logger;
	std::string _Method;
	std::string _Path;
	std::string _Query;
	HttpVersion _Version;
	HttpFieldMap _Headers;
	std::vector<uint8_t> _Body;

	bool _IsRequestLineParsed;
	bool _IsRequestHeaderParsed;
	bool _IsParseCompleted;
	size_t _ContentLength;
	std::string _Host;
	bool _IsChunkedRequest;
	std::string _NormalizedPath;

	// 正規化されて分割されたパスが入る
	std::vector<std::string> _PathSegmentList;
	ServerRunningConfig *serverRunningConfig;

	std::vector<uint8_t> _UnparsedRequestRaw;
	// chunkは後で実装

 public:
	HttpRequest(const Logger &logger);
	HttpRequest(const HttpRequest &src);
	virtual ~HttpRequest();

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
	const HttpVersion &getVersion() const;
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
	const std::vector<std::string> &getPathSegmentList() const;
	const ServerRunningConfig &getServerRunningConfig() const;
	void setServerRunningConfig(const ServerRunningConfig &serverRunningConfig);

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
