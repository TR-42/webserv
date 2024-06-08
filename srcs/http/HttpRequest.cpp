#include <cstring>
#include <http/HttpRequest.hpp>
#include <http/exception/BadRequest.hpp>
#include <http/exception/NotImplemented.hpp>
#include <http/exception/RequestEntityTooLarge.hpp>
#include <iostream>
#include <stdexcept>
#include <utils/normalizePath.hpp>
#include <utils/pickLine.hpp>
#include <utils/split.hpp>
#include <utils/splitNameValue.hpp>
#include <utils/strcasecmp.hpp>

#include "http/HttpFieldMap.hpp"

#define REQUEST_MAX_SIZE (512 * 1024 * 1024)

#define METHOD_GET "GET"
#define METHOD_HEAD "HEAD"
#define METHOD_POST "POST"
#define METHOD_PUT "DELETE"

namespace webserv
{

const std::string &HttpRequest::getMethod() const { return this->_Method; }
const std::string &HttpRequest::getPath() const { return this->_Path; }
const std::string &HttpRequest::getQuery() const { return this->_Query; }
const HttpVersion &HttpRequest::getVersion() const { return this->_Version; }
const HttpFieldMap &HttpRequest::getHeaders() const { return this->_Headers; }
const MessageBody &HttpRequest::getBody() const { return this->_Body; }
bool HttpRequest::isRequestLineParsed() const { return this->_IsRequestLineParsed; }
bool HttpRequest::isRequestHeaderParsed() const { return this->_IsRequestHeaderParsed; }

HttpRequest::HttpRequest(
	const Logger &logger
) : logger(logger),
		_TotalRequestSize(0),
		_Version(1, 1),
		_IsRequestLineParsed(false),
		_IsRequestHeaderParsed(false),
		serverRunningConfig(NULL)
{
}

HttpRequest::HttpRequest(
	const HttpRequest &src
) : logger(src.logger),
		_TotalRequestSize(src._TotalRequestSize),
		_Method(src._Method),
		_Path(src._Path),
		_Query(src._Query),
		_Version(src._Version),
		_Headers(src._Headers),
		_Body(src._Body),
		_IsRequestLineParsed(src._IsRequestLineParsed),
		_IsRequestHeaderParsed(src._IsRequestHeaderParsed),
		_Host(src._Host),
		_NormalizedPath(src._NormalizedPath),
		serverRunningConfig(new ServerRunningConfig(*src.serverRunningConfig))
{
}

HttpRequest::~HttpRequest()
{
	if (this->serverRunningConfig != NULL) {
		delete this->serverRunningConfig;
	}
}

bool HttpRequest::pushRequestRaw(
	const std::vector<uint8_t> &requestRaw
)
{
	if (requestRaw.size() == 0) {
		C_DEBUG("requestRaw was empty");
		return true;
	}

	if (this->isParseCompleted()) {
		C_DEBUG("Request parsing was already completed");
		return true;
	}

	// バッファサイズ的にオーバーフローは考えられないため、チェックしない
	this->_TotalRequestSize += requestRaw.size();
	if (REQUEST_MAX_SIZE < this->_TotalRequestSize) {
		C_WARN("Request size is too large");
		throw http::exception::RequestEntityTooLarge();
	}

	if (this->_IsRequestHeaderParsed == false) {
		C_DEBUG("_IsRequestHeaderParsed was false");
		this->_UnparsedRequestRaw.insert(this->_UnparsedRequestRaw.end(), requestRaw.begin(), requestRaw.end());

		C_DEBUG("pickLine executing...");
		std::vector<uint8_t> *requestRawLine = utils::pickLine(this->_UnparsedRequestRaw);
		if (requestRawLine == NULL) {
			C_DEBUG("NewLine not found");
			return true;
		}
		CS_DEBUG() << "requestRawLine size: " << requestRawLine->size() << std::endl;

		// リクエストラインの解析
		if (this->_IsRequestLineParsed == false) {
			C_DEBUG("_IsRequestLineParsed was false");
			this->_IsRequestLineParsed = this->parseRequestLine(*requestRawLine);
			delete requestRawLine;
			CS_DEBUG() << "_IsRequestLineParsed result: " << this->_IsRequestLineParsed << std::endl;
			if (this->_IsRequestLineParsed == false) {
				return false;
			}
			if (this->_Version < HttpVersion(1, 0)) {
				this->_IsRequestHeaderParsed = true;
				C_INFO("HTTP/0.9 -> No Header/NoBody");
				return true;
			}

			requestRawLine = utils::pickLine(this->_UnparsedRequestRaw);
			if (requestRawLine == NULL) {
				C_DEBUG("NewLine not found");
				return true;
			}
		}

		// リクエストヘッダの解析
		while (requestRawLine->empty() == false) {
			bool result = this->parseRequestHeader(*requestRawLine);
			delete requestRawLine;
			if (result == false) {
				return false;
			}
			requestRawLine = utils::pickLine(this->_UnparsedRequestRaw);
			if (requestRawLine == NULL) {
				C_DEBUG("NewLine not found");
				return true;
			}
			CS_DEBUG() << "requestRawLine size: " << requestRawLine->size() << std::endl;
		}
		delete requestRawLine;

		if (this->_Headers.isNameExists("Host")) {
			std::vector<std::string> hostList = this->_Headers.getValueList("Host");
			if (hostList.size() != 1) {
				C_WARN("Host header is not unique");
				return false;
			}

			if (hostList[0].empty()) {
				C_WARN("Host header is empty");
				return false;
			}

			this->_Host = hostList[0];
			size_t colonPos = this->_Host.find(':');
			if (colonPos != std::string::npos) {
				this->_Host = this->_Host.substr(0, colonPos);
			}
		} else {
			throw http::exception::BadRequest();
		}

		this->_IsRequestHeaderParsed = true;

		this->_Body = MessageBody::init(this->_Headers, false);

		CS_INFO()
			<< "Request Header Parse Completed:"
			<< " Method: " << this->_Method
			<< ", Path: `" << this->_Path << "`"
			<< ", Version: " << this->_Version.toString()
			<< ", IsParseCompleted: " << std::boolalpha << this->isParseCompleted()
			<< ", ContentLength: " << this->_Body.getContentLength()
			<< ", Chunked: " << std::boolalpha << this->_Body.getIsChunked()
			<< ", Host: " << this->_Host
			<< std::endl;

		return this->_Body.pushData(this->_UnparsedRequestRaw.data(), this->_UnparsedRequestRaw.size());
	} else {
		return this->_Body.pushData(requestRaw.data(), requestRaw.size());
	}

	C_DEBUG("return true");

	return true;
}

static void separatePathAndQuery(
	std::string &path,
	std::string &query
)
{
	// ハッシュはサーバに送信されないため、存在を確認しない
	size_t queryPos = path.find('?');
	if (queryPos == std::string::npos) {
		query = "";
	} else {
		bool isQueryEmpty = queryPos + 1 == path.size();
		query = isQueryEmpty ? "" : path.substr(queryPos + 1);
		path = path.substr(0, queryPos);
	}
}

bool HttpRequest::parseRequestLine(
	const std::vector<uint8_t> &requestRawLine
)
{
	const uint8_t *requestRawData = requestRawLine.data();
	size_t newlinePos = requestRawLine.size();
	const uint8_t *spacePos1 = (const uint8_t *)std::memchr(requestRawData, ' ', newlinePos);
	if (spacePos1 == NULL) {
		C_WARN("spacePos1 was NULL");
		return false;
	}
	C_DEBUG("spacePos1 was not null");
	size_t lenToSpacePos1 = spacePos1 - requestRawData;
	if (lenToSpacePos1 == 0) {
		C_WARN("lenToSpacePos1 was 0");
		return false;
	}
	CS_DEBUG() << "lenToSpacePos1: " << lenToSpacePos1 << std::endl;
	this->_Method = std::string((const char *)requestRawData, lenToSpacePos1);
	CS_DEBUG() << "Method: " << this->_Method << std::endl;

	if (this->_Method != METHOD_GET && this->_Method != METHOD_HEAD && this->_Method != METHOD_POST && this->_Method != METHOD_PUT) {
		C_WARN("Invalid Method");
		throw http::exception::NotImplemented();
	}

	const uint8_t *pathSegment = spacePos1 + 1;
	const uint8_t *spacePos2 = (const uint8_t *)std::memchr(pathSegment, ' ', newlinePos - (lenToSpacePos1 + 1));
	if (spacePos2 == NULL) {
		C_WARN("spacePos2 was NULL -> may be HTTP/0.9");
		spacePos2 = requestRawData + newlinePos;
	} else {
		C_DEBUG("spacePos2 was not null");
	}
	size_t lenToSpacePos2 = spacePos2 - pathSegment;
	if (lenToSpacePos2 == 0) {
		C_WARN("lenToSpacePos2 was 0");
		return false;
	}
	CS_DEBUG() << "lenToSpacePos2: " << lenToSpacePos2 << std::endl;
	this->_Path = std::string((const char *)pathSegment, lenToSpacePos2);
	CS_DEBUG() << "PathSegment: `" << this->_Path << "`" << std::endl;
	separatePathAndQuery(this->_Path, this->_Query);
	this->_NormalizedPath = utils::normalizePath(this->_Path);
	this->_PathSegmentList = utils::split(this->_NormalizedPath, '/');
	CS_DEBUG()
		<< "Path: `" << this->_Path << "`, "
		<< "Query: `" << this->_Query << "`, "
		<< "NormalizedPath: `" << this->_NormalizedPath << "`"
		<< std::endl;

	if (spacePos2 == (requestRawData + newlinePos)) {
		C_WARN("spacePos2 was NULL -> may be HTTP/0.9");
		this->_Version = HttpVersion(0, 9);
		if (this->_Method != METHOD_GET) {
			throw http::exception::BadRequest();
		}
		return true;
	}
	const uint8_t *versionSegment = spacePos2 + 1;
	size_t versionStringLength = newlinePos - lenToSpacePos2 - lenToSpacePos1 - 2;
	CS_DEBUG() << "versionStringLength: " << versionStringLength << std::endl;
	if (std::memchr(versionSegment, ' ', versionStringLength) != NULL) {
		C_WARN("versionSegment contains space");
		return false;
	}
	C_DEBUG("versionSegment does not contain space");
	if (versionStringLength == 0) {
		C_WARN("versionStringLength was 0");
		return false;
	}
	std::string versionString((const char *)versionSegment, versionStringLength);
	this->_Version = HttpVersion::fromString(versionString);
	CS_DEBUG() << "Version: " << this->_Version.toString() << std::endl;

	if (HttpVersion(2, 0) <= this->_Version) {
		throw http::exception::HttpVersionNotSupported();
	}

	return true;
}

bool HttpRequest::parseRequestHeader(
	const std::vector<uint8_t> &requestRawLine
)
{
	std::pair<std::string, std::string> nameValue = utils::splitNameValue(requestRawLine);
	if (nameValue.first.empty()) {
		C_WARN("nameValue.first was empty");
		return false;
	}
	if (utils::strcasecmp(nameValue.first, "Transfer-Encoding")) {
		if (nameValue.second != "chunked") {
			C_WARN("Transfer-Encoding is not chunked");
			throw http::exception::NotImplemented();
		}
	}

	this->_Headers.addValue(nameValue.first, nameValue.second);

	return true;
}

bool webserv::HttpRequest::isParseCompleted() const
{
	return this->_IsRequestHeaderParsed && this->_Body.getIsProcessComplete();
}

std::string HttpRequest::getHost() const
{
	return this->_Host;
}

std::string HttpRequest::getNormalizedPath() const
{
	return this->_NormalizedPath;
}

const std::vector<std::string> &HttpRequest::getPathSegmentList() const
{
	return this->_PathSegmentList;
}

bool HttpRequest::isServerRunningConfigSet() const
{
	return this->serverRunningConfig != NULL;
}

const ServerRunningConfig &HttpRequest::getServerRunningConfig() const
{
	return *this->serverRunningConfig;
}

void HttpRequest::setServerRunningConfig(const ServerRunningConfig &serverRunningConfig)
{
	if (this->serverRunningConfig != NULL) {
		delete this->serverRunningConfig;
	}
	this->serverRunningConfig = new ServerRunningConfig(serverRunningConfig);
}

void HttpRequest::setPath(
	const std::string &path
)
{
	this->_Path = path;
	separatePathAndQuery(this->_Path, this->_Query);
	this->_NormalizedPath = utils::normalizePath(this->_Path);
	this->_PathSegmentList = utils::split(this->_NormalizedPath, '/');
	CS_DEBUG()
		<< "Path: `" << this->_Path << "`, "
		<< "Query: `" << this->_Query << "`, "
		<< "NormalizedPath: `" << this->_NormalizedPath << "`"
		<< std::endl;
}

}	 // namespace webserv
