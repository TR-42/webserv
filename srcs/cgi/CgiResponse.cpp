#include "cgi/CgiResponse.hpp"

#include <unistd.h>

#include <iostream>
#include <utils/pickLine.hpp>
#include <utils/splitNameValue.hpp>
#include <utils/splitWithSpace.hpp>
#include <utils/strcasecmp.hpp>

#include "cgi/CgiResponseMode.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "service/ServiceEventResult.hpp"
#include "utils/ErrorPageProvider.hpp"

namespace webserv
{

CgiResponse::CgiResponse(
	const Logger &logger,
	const utils::ErrorPageProvider &errorPageProvider
) : logger(logger),
		_mode(CgiResponseMode::DOCUMENT),
		_ContentType(""),
		_Location(""),
		_StatusCode("200"),
		_ReasonPhrase("OK"),
		_IsResponseHeaderParsed(false),
		isSetContentType(false),
		isSetLocation(false),
		isSetStatus(false),
		isSetProtocolFieldMap(false),
		isSetExtensionFieldMap(false),
		isSetResponseBody(false),
		isAbsolutePath(false),
		_errorPageProvider(errorPageProvider)
{
}

CgiResponse::~CgiResponse()
{
}

CgiResponse::CgiResponse(
	const CgiResponse &other
) : logger(other.logger),
		_mode(other._mode),
		_ContentType(other._ContentType),
		_Location(other._Location),
		_StatusCode(other._StatusCode),
		_ReasonPhrase(other._ReasonPhrase),
		_ProtocolFieldMap(other._ProtocolFieldMap),
		_ExtensionFieldMap(other._ExtensionFieldMap),
		_IsResponseHeaderParsed(other._IsResponseHeaderParsed),
		isSetContentType(other.isSetContentType),
		isSetLocation(other.isSetLocation),
		isSetStatus(other.isSetStatus),
		isSetProtocolFieldMap(other.isSetProtocolFieldMap),
		isSetExtensionFieldMap(other.isSetExtensionFieldMap),
		isSetResponseBody(other.isSetResponseBody),
		isAbsolutePath(other.isAbsolutePath),
		_UnparsedResponseRaw(other._UnparsedResponseRaw),
		_errorPageProvider(other._errorPageProvider)
{
}

CgiResponse &CgiResponse::operator=(const CgiResponse &other)
{
	if (this == &other) {
		return *this;
	}

	_mode = other._mode;
	_ContentType = other._ContentType;
	_Location = other._Location;
	_StatusCode = other._StatusCode;
	_ReasonPhrase = other._ReasonPhrase;
	_ProtocolFieldMap = other._ProtocolFieldMap;
	_ExtensionFieldMap = other._ExtensionFieldMap;

	return *this;
}

std::vector<uint8_t> CgiResponse::generateResponsePacket(
	bool withBody
) const
{
	return this->getHttpResponse().generateResponsePacket(withBody, false);
}

HttpResponse CgiResponse::getHttpResponse() const
{
	if (!this->_IsResponseHeaderParsed) {
		return this->_errorPageProvider.internalServerError();
	}
	if (this->_mode == CgiResponseMode::LOCAL_REDIRECT) {
		return this->_errorPageProvider.internalServerError();
	}

	if (this->_mode == CgiResponseMode::CLIENT_REDIRECT && !this->_ResponseBody.empty()) {
		return this->_errorPageProvider.internalServerError();
	}

	if (!this->_ResponseBody.getIsEndWithEOF() && !this->_ResponseBody.getIsProcessComplete()) {
		return this->_errorPageProvider.internalServerError();
	}

	HttpResponse httpResponse;
	httpResponse.setVersion("HTTP/1.1");
	httpResponse.setStatusCode(this->_StatusCode);
	httpResponse.setReasonPhrase(this->_ReasonPhrase);
	httpResponse.setHeaders(this->_ProtocolFieldMap);
	httpResponse.setBody(this->_ResponseBody.getBody());

	if (this->isSetContentType) {
		httpResponse.getHeaders().addValue("Content-Type", this->_ContentType);
	}

	if (this->_mode == CgiResponseMode::CLIENT_REDIRECT || this->_mode == CgiResponseMode::CLIENT_REDIRECT_WITH_DOCUMENT) {
		httpResponse.getHeaders().addValue("Location", this->_Location);
	}

	return httpResponse;
}

bool CgiResponse::pushResponseRaw(
	const std::vector<uint8_t> &responseRaw
)
{
	return this->pushResponseRaw(responseRaw.data(), responseRaw.size());
}
bool webserv::CgiResponse::pushResponseRaw(const uint8_t *responseRaw, size_t responseRawSize)
{
	if (this->_IsResponseHeaderParsed) {
		if (this->_mode != CgiResponseMode::DOCUMENT && this->_mode != CgiResponseMode::CLIENT_REDIRECT_WITH_DOCUMENT) {
			return false;
		}

		return this->_ResponseBody.pushData(responseRaw, responseRawSize);
	}

	this->_UnparsedResponseRaw.insert(
		_UnparsedResponseRaw.end(),
		responseRaw,
		responseRaw + responseRawSize
	);

	while (true) {
		std::vector<uint8_t> *responseRawLine = utils::pickLine(_UnparsedResponseRaw);
		if (responseRawLine == NULL) {
			break;
		}

		if (responseRawLine->empty()) {
			_IsResponseHeaderParsed = true;
			delete responseRawLine;

			CS_DEBUG()
				<< "isSetContentType: " << std::boolalpha << this->isSetContentType
				<< ", isSetLocation: " << std::boolalpha << this->isSetLocation
				<< ", isSetStatus: " << std::boolalpha << this->isSetStatus
				<< ", isSetProtocolFieldMap: " << std::boolalpha << this->isSetProtocolFieldMap
				<< ", isSetExtensionFieldMap: " << std::boolalpha << this->isSetExtensionFieldMap
				<< ", isSetResponseBody: " << std::boolalpha << this->isSetResponseBody
				<< ", isAbsolutePath: " << std::boolalpha << this->isAbsolutePath
				<< std::endl;

			if (
				this->isSetContentType &&
				!this->isSetLocation
			) {
				this->_mode = CgiResponseMode::DOCUMENT;
				this->_ResponseBody = MessageBody::init(this->_ProtocolFieldMap, true);
				this->_ProtocolFieldMap.isNameExists("Content-Length");
				this->_ProtocolFieldMap.isNameExists("Transfer-Encoding");
				bool pushResult = this->_ResponseBody.pushData(this->_UnparsedResponseRaw.data(), this->_UnparsedResponseRaw.size());
				this->_UnparsedResponseRaw.clear();
				return pushResult;
			}

			// location以外存在しないことを確認
			else if (
				this->isSetLocation &&
				!this->isSetStatus &&
				!this->isSetProtocolFieldMap &&
				!this->isSetExtensionFieldMap &&
				!this->isSetResponseBody &&
				!this->isSetContentType &&
				this->isAbsolutePath
			) {
				this->_mode = CgiResponseMode::LOCAL_REDIRECT;
				if (!this->_UnparsedResponseRaw.empty()) {
					return false;
				}
			}

			else if (
				!this->isSetProtocolFieldMap &&
				!this->isSetStatus &&
				!this->isSetContentType &&
				!this->isAbsolutePath &&
				this->isSetLocation
			) {
				this->_mode = CgiResponseMode::CLIENT_REDIRECT;
				this->_StatusCode = "301";
				this->_ReasonPhrase = "Moved Permanently";
				if (!this->_UnparsedResponseRaw.empty()) {
					return false;
				}
			}

			else if (
				this->isSetStatus &&
				this->isSetContentType &&
				this->isSetLocation &&
				!this->isAbsolutePath
			) {
				this->_mode = CgiResponseMode::CLIENT_REDIRECT_WITH_DOCUMENT;
				this->_ResponseBody = MessageBody::init(this->_ProtocolFieldMap, true);
				this->_ProtocolFieldMap.isNameExists("Content-Length");
				this->_ProtocolFieldMap.isNameExists("Transfer-Encoding");
				bool pushResult = this->_ResponseBody.pushData(this->_UnparsedResponseRaw.data(), this->_UnparsedResponseRaw.size());
				this->_UnparsedResponseRaw.clear();
				return pushResult;
			}

			else {
				return false;
			}
			break;
		}

		bool isParseSuccess = parseResponseHeader(*responseRawLine);
		delete responseRawLine;
		if (!isParseSuccess) {
			return false;
		}
	}

	return true;
}

bool CgiResponse::parseResponseHeader(
	const std::vector<uint8_t> &responseRawLine
)
{
	std::pair<std::string, std::string> nameValue = utils::splitNameValue(responseRawLine);
	if (nameValue.first.empty()) {
		C_WARN("nameValue.first was empty");
		return false;
	}

	if (utils::strcasecmp(nameValue.first, "content-type")) {
		if (this->isSetContentType) {
			C_WARN("Content-Type already set");
			return false;
		}

		this->isSetContentType = true;
		_ContentType = nameValue.second;
		return true;
	}

	if (utils::strcasecmp(nameValue.first, "location")) {
		if (this->isSetLocation) {
			C_WARN("Location already set");
			return false;
		}

		this->isSetLocation = true;
		if (nameValue.second.empty()) {
			C_WARN("nameValue.second was empty");
			return false;
		}
		this->isAbsolutePath = nameValue.second[0] == '/';
		_Location = nameValue.second;
		return true;
	}

	if (utils::strcasecmp(nameValue.first, "status")) {
		if (this->isSetStatus) {
			C_WARN("Status already set");
			return false;
		}

		std::vector<std::string> statusParts = utils::splitWithSpace(nameValue.second, 2);

		if (statusParts.size() < 2) {
			C_WARN("statusParts.size() < 2");
			return false;
		}
		isSetStatus = true;
		_StatusCode = statusParts[0];
		_ReasonPhrase = statusParts[1];

		return true;
	}

	if (utils::strcasecmp(nameValue.first.substr(0, 6), "x-cgi-")) {
		_ExtensionFieldMap.addValue(nameValue.first, nameValue.second);
		return true;
	}

	_ProtocolFieldMap.addValue(nameValue.first, nameValue.second);

	return true;
}

CgiResponseModeType CgiResponse::getMode() const
{
	return this->_mode;
}

const std::string &CgiResponse::getContentType() const
{
	return this->_ContentType;
}

const std::string &CgiResponse::getLocation() const
{
	return this->_Location;
}

const std::string &CgiResponse::getStatusCode() const
{
	return this->_StatusCode;
}

const std::string &CgiResponse::getReasonPhrase() const
{
	return this->_ReasonPhrase;
}

const HttpFieldMap &CgiResponse::getProtocolFieldMap() const
{
	return this->_ProtocolFieldMap;
}

const HttpFieldMap &CgiResponse::getExtensionFieldMap() const
{
	return this->_ExtensionFieldMap;
}

const MessageBody &CgiResponse::getResponseBody() const
{
	return this->_ResponseBody;
}

}	 // namespace webserv
