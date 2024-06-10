#pragma once

#include <Logger.hpp>
#include <map>
#include <stdexcept>
#include <string>
#include <types.hpp>
#include <vector>

#include "./HttpVersion.hpp"
#include "./MessageBody.hpp"
#include "config/ServerRunningConfig.hpp"
#include "http/HttpFieldMap.hpp"
#include "service/RequestedFileInfo.hpp"

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
	size_t _TotalRequestSize;
	std::string _Method;
	std::string _Path;
	std::string _Query;
	HttpVersion _Version;
	HttpFieldMap _Headers;
	MessageBody _Body;

	bool _IsRequestLineParsed;
	bool _IsRequestHeaderParsed;
	std::string _Host;
	std::string _NormalizedPath;

	// 正規化されて分割されたパスが入る
	std::vector<std::string> _PathSegmentList;
	ServerRunningConfig *serverRunningConfig;
	RequestedFileInfo *_requestedFileInfo;
	HttpRouteConfig _routeConfig;

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
	bool pushRequestRaw(
		const uint8_t *requestRaw,
		size_t requestRawSize
	);

	const std::string &getMethod() const;
	const std::string &getPath() const;
	const std::string &getQuery() const;
	const HttpVersion &getVersion() const;
	const HttpFieldMap &getHeaders() const;
	const MessageBody &getBody() const;
	bool isRequestLineParsed() const;
	bool isRequestHeaderParsed() const;
	bool isParseCompleted() const;
	std::string getHost() const;
	std::string getNormalizedPath() const;
	const std::vector<std::string> &getPathSegmentList() const;
	bool isServerRunningConfigSet() const;
	bool isRequestedFileInfoSet() const;
	const ServerRunningConfig &getServerRunningConfig() const;
	const RequestedFileInfo &getRequestedFileInfo() const;
	void setServerRunningConfig(const ServerRunningConfigListType &serverRunningConfigList);

	void updatePath(const std::string &path);

	inline const HttpRouteConfig &getRouteConfig() const { return this->_routeConfig; }
	inline size_t getTotalRequestSize() const { return this->_TotalRequestSize; }

	bool isSizeLimitExceeded() const;

 private:
	bool
	parseRequestLine(
		const std::vector<uint8_t> &requestRawLine
	);

	bool parseRequestHeader(
		const std::vector<uint8_t> &requestRawLine
	);

	std::string requestText;
	void setPath(const std::string &path);
	bool reloadRouteConfig()
	{
		if (this->serverRunningConfig == NULL) {
			return false;
		}
		this->_routeConfig = this->serverRunningConfig->pickRouteConfig(
			this->_PathSegmentList,
			this->_Method
		);
		if (this->_requestedFileInfo != NULL) {
			delete this->_requestedFileInfo;
			this->_requestedFileInfo = NULL;
		}
		this->_requestedFileInfo = new RequestedFileInfo(
			this->_PathSegmentList,
			this->_Path[this->_Path.length() - 1] == '/',
			this->_routeConfig,
			this->logger
		);
		return true;
	}
};

}	 // namespace webserv
