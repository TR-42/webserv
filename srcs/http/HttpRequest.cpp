#include <cstring>
#include <http/HttpRequest.hpp>
#include <iostream>

#include "utils.hpp"

namespace webserv
{

const std::string &HttpRequest::getMethod() const { return _Method; }
const std::string &HttpRequest::getPath() const { return _Path; }
const std::string &HttpRequest::getVersion() const { return _Version; }
const RequestHeaderMap &HttpRequest::getHeaders() const { return _Headers; }
const std::vector<uint8_t> &HttpRequest::getBody() const { return _Body; }
bool HttpRequest::isRequestLineParsed() const { return _IsRequestLineParsed; }
bool HttpRequest::isRequestHeaderParsed() const { return _IsRequestHeaderParsed; }

static std::vector<uint8_t> *_pickLine(
	std::vector<uint8_t> &unparsedRequestRaw
)
{
	size_t newlinePos;
	size_t unparsedRequestRawSize = unparsedRequestRaw.size();
	std::vector<uint8_t> *requestRawLine;
	for (newlinePos = 0; newlinePos < unparsedRequestRawSize; ++newlinePos) {
		if (unparsedRequestRaw[newlinePos] == '\n') {
			break;
		}
	}
	if (newlinePos == unparsedRequestRawSize) {
		return NULL;
	}
	bool hasCarriageReturn = 0 < newlinePos && unparsedRequestRaw[newlinePos - 1] == '\r';
	requestRawLine = new std::vector<uint8_t>(
		unparsedRequestRaw.begin(),
		unparsedRequestRaw.begin() + newlinePos - (hasCarriageReturn ? 1 : 0)
	);
	unparsedRequestRaw.erase(unparsedRequestRaw.begin(), unparsedRequestRaw.begin() + newlinePos + 1);
	return requestRawLine;
}

HttpRequest::HttpRequest()
		: _IsRequestLineParsed(false),
			_IsRequestHeaderParsed(false)
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

	_UnparsedRequestRaw.insert(_UnparsedRequestRaw.end(), requestRaw.begin(), requestRaw.end());

	if (_IsRequestHeaderParsed == false) {
		C_DEBUG("_IsRequestHeaderParsed was false");

		C_INFO("pickLine executing...");
		std::vector<uint8_t> *requestRawLine = _pickLine(_UnparsedRequestRaw);
		if (requestRawLine == NULL) {
			C_INFO("NewLine not found");
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
				return false;
			}
			requestRawLine = _pickLine(_UnparsedRequestRaw);
			if (requestRawLine == NULL) {
				C_INFO("NewLine not found");
				return true;
			}
		}

		// リクエストヘッダの解析
		while (requestRawLine->empty() == false) {
			bool result = parseRequestHeader(*requestRawLine);
			delete requestRawLine;
			if (result == false) {
				return false;
			}
			requestRawLine = _pickLine(_UnparsedRequestRaw);
			if (requestRawLine == NULL) {
				C_INFO("NewLine not found");
				return true;
			}
			CS_DEBUG() << "requestRawLine size: " << requestRawLine->size() << std::endl;
		}
		_IsRequestHeaderParsed = true;
		delete requestRawLine;
		CS_DEBUG() << "_IsRequestHeaderParsed result: " << _IsRequestHeaderParsed << std::endl;
		return _IsRequestHeaderParsed;
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
	C_INFO("spacePos1 was not null");
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
	C_INFO("spacePos2 was not null");
	size_t lenToSpacePos2 = spacePos2 - pathSegment;
	if (lenToSpacePos2 == 0) {
		C_WARN("lenToSpacePos2 was 0");
		return false;
	}
	CS_DEBUG() << "lenToSpacePos2: " << lenToSpacePos2 << std::endl;
	_Path = std::string((const char *)pathSegment, lenToSpacePos2);
	CS_DEBUG() << "Path: " << _Path << std::endl;
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
	const uint8_t *requestRawData = requestRawLine.data();
	size_t newlinePos = requestRawLine.size();
	const uint8_t *separatorPos = (const uint8_t *)std::memchr(requestRawData, ':', newlinePos);
	if (separatorPos == NULL) {
		C_WARN("separatorPos was NULL");
		return false;
	}
	C_INFO("separatorPos was not null");
	size_t lenToSeparatorPos = separatorPos - requestRawData;
	if (lenToSeparatorPos == 0) {
		return false;
	}
	std::string _Key = std::string((const char *)requestRawData, lenToSeparatorPos);
	size_t keyLen = _Key.size();
	CS_DEBUG() << "keyLen: " << keyLen << " Key: " << _Key << std::endl;
	// TODO: keyのバリデーションの実装
	if (std::isspace(_Key[keyLen - 1])) {
		C_DEBUG("isspace(_Key[keyLen - 1]) was true");
		return false;
	}
	std::string _Value = utils::strtrim(std::string((const char *)separatorPos + 1, newlinePos - lenToSeparatorPos - 1));
	CS_DEBUG() << "Value: " << _Value << std::endl;
	if (_Headers.find(_Key) == _Headers.end()) {
		_Headers[_Key] = std::vector<std::string>();
		CS_DEBUG() << "Key: " << _Key << " was not found" << std::endl;
	}
	_Headers[_Key].push_back(_Value);
	return true;
}

}	 // namespace webserv