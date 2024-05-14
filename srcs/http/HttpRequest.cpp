#include <cstring>
#include <http/HttpRequest.hpp>
#include <iostream>

#include "http/HttpFieldMap.hpp"
#include "utils.hpp"

namespace webserv
{

const std::string &HttpRequest::getMethod() const { return _Method; }
const std::string &HttpRequest::getPath() const { return _Path; }
const std::string &HttpRequest::getVersion() const { return _Version; }
const HttpFieldMap &HttpRequest::getHeaders() const { return _Headers; }
const std::vector<uint8_t> &HttpRequest::getBody() const { return _Body; }
bool HttpRequest::isRequestLineParsed() const { return _IsRequestLineParsed; }
bool HttpRequest::isRequestHeaderParsed() const { return _IsRequestHeaderParsed; }

HttpRequest::HttpRequest()
		: _IsRequestLineParsed(false),
			_IsRequestHeaderParsed(false),
			_IsParseCompleted(false),
			_ContentLength(0),
			_IsChunkedRequest(false)
{
}

bool HttpRequest::pushRequestRaw(
	const std::vector<uint8_t> &requestRaw
)
{
	if (requestRaw.size() == 0) {
		C_DEBUG("requestRaw was empty");
		return true;
	}

	if (this->_IsParseCompleted) {
		C_DEBUG("Request parsing was already completed");
		return true;
	}

	if (_IsRequestHeaderParsed == false) {
		C_DEBUG("_IsRequestHeaderParsed was false");
		_UnparsedRequestRaw.insert(_UnparsedRequestRaw.end(), requestRaw.begin(), requestRaw.end());

		C_DEBUG("pickLine executing...");
		std::vector<uint8_t> *requestRawLine = utils::pickLine(_UnparsedRequestRaw);
		if (requestRawLine == NULL) {
			C_DEBUG("NewLine not found");
			return true;
		}
		CS_DEBUG() << "requestRawLine size: " << requestRawLine->size() << std::endl;

		// リクエストラインの解析
		if (_IsRequestLineParsed == false) {
			C_DEBUG("_IsRequestLineParsed was false");
			_IsRequestLineParsed = parseRequestLine(*requestRawLine);
			delete requestRawLine;
			CS_DEBUG() << "_IsRequestLineParsed result: " << _IsRequestLineParsed << std::endl;
			if (_IsRequestLineParsed == false) {
				this->_IsParseCompleted = true;
				return false;
			}
			requestRawLine = utils::pickLine(_UnparsedRequestRaw);
			if (requestRawLine == NULL) {
				C_DEBUG("NewLine not found");
				return true;
			}
		}

		// リクエストヘッダの解析
		while (requestRawLine->empty() == false) {
			bool result = parseRequestHeader(*requestRawLine);
			delete requestRawLine;
			if (result == false) {
				this->_IsParseCompleted = true;
				return false;
			}
			requestRawLine = utils::pickLine(_UnparsedRequestRaw);
			if (requestRawLine == NULL) {
				C_DEBUG("NewLine not found");
				return true;
			}
			CS_DEBUG() << "requestRawLine size: " << requestRawLine->size() << std::endl;
		}
		delete requestRawLine;

		bool tryGetContentLengthResult = this->_Headers.tryGetContentLength(this->_ContentLength);
		CS_DEBUG()
			<< "tryGetContentLength result: " << std::boolalpha << tryGetContentLengthResult
			<< std::endl;

		// TODO: HTTP/1系でHostが指定されていない場合に400を返す
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
		}

		_IsRequestHeaderParsed = true;

		_Body = _UnparsedRequestRaw;
		// TODO: chunkedの処理を実装する
		this->_IsParseCompleted = this->_ContentLength <= _Body.size();

		CS_INFO()
			<< "Request Header Parse Completed:"
			<< " Method: " << _Method
			<< ", Path: `" << _Path << "`"
			<< ", Version: " << _Version
			<< ", IsParseCompleted: " << std::boolalpha << this->_IsParseCompleted
			<< ", ContentLength: " << _ContentLength
			<< ", Host: " << _Host
			<< ", IsChunkedRequest: " << std::boolalpha << this->_IsChunkedRequest
			<< std::endl;

		return true;
	} else if (isRequestBodyLengthEnough()) {
		CS_DEBUG()
			<< "too much request body("
			<< "BodySize: " << _Body.size() << ", "
			<< "ContentLength: " << getContentLength() << ", "
			<< "RequestRawSize: " << requestRaw.size()
			<< ")" << std::endl;
		this->_IsParseCompleted = true;
		return false;
	} else {
		_Body.insert(_Body.end(), requestRaw.begin(), requestRaw.end());
		return true;
	}

	C_DEBUG("return true");

	return true;
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
	_Method = std::string((const char *)requestRawData, lenToSpacePos1);
	CS_DEBUG() << "Method: " << _Method << std::endl;
	const uint8_t *pathSegment = spacePos1 + 1;
	const uint8_t *spacePos2 = (const uint8_t *)std::memchr(pathSegment, ' ', newlinePos - (lenToSpacePos1 + 1));
	if (spacePos2 == NULL) {
		C_WARN("spacePos2 was NULL");
		return false;
	}
	C_DEBUG("spacePos2 was not null");
	size_t lenToSpacePos2 = spacePos2 - pathSegment;
	if (lenToSpacePos2 == 0) {
		C_WARN("lenToSpacePos2 was 0");
		return false;
	}
	CS_DEBUG() << "lenToSpacePos2: " << lenToSpacePos2 << std::endl;
	_Path = std::string((const char *)pathSegment, lenToSpacePos2);
	this->_NormalizedPath = utils::normalizePath(_Path);
	CS_DEBUG()
		<< "Path: `" << _Path << "`, "
		<< "NormalizedPath: `" << this->_NormalizedPath << "`"
		<< std::endl;
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
	_Version = std::string((const char *)versionSegment, versionStringLength);
	CS_DEBUG() << "Version: " << _Version << std::endl;
	return true;
}

bool HttpRequest::parseRequestHeader(
	const std::vector<uint8_t> &requestRawLine
)
{
	std::pair<std::string, std::string> nameValue = utils::splitNameValue(requestRawLine, ':');
	if (nameValue.first.empty()) {
		C_WARN("nameValue.first was empty");
		return false;
	}
	this->_Headers.addValue(nameValue.first, nameValue.second);

	return true;
}

size_t HttpRequest::getContentLength() const
{
	return this->_ContentLength;
}

bool HttpRequest::isRequestBodyLengthEnough() const
{
	bool isRequestBodyLengthEnough = _IsRequestHeaderParsed && getContentLength() <= _Body.size();
	return isRequestBodyLengthEnough;
}

bool HttpRequest::isRequestBodyLengthTooMuch() const
{
	bool isRequestBodyLengthTooMuch = _IsRequestHeaderParsed && getContentLength() < _Body.size();
	return isRequestBodyLengthTooMuch;
}

bool webserv::HttpRequest::isParseCompleted()
{
	if (!this->_IsParseCompleted) {
		this->_IsParseCompleted = this->isRequestBodyLengthEnough();
	}
	return this->_IsParseCompleted;
}

std::string HttpRequest::getHost() const
{
	return this->_Host;
}

bool HttpRequest::isChunkedRequest() const
{
	return this->_IsChunkedRequest;
}

std::string HttpRequest::getNormalizedPath() const
{
	return this->_NormalizedPath;
}

}	 // namespace webserv
