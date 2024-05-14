#include "cgi/CgiResponse.hpp"

#include <unistd.h>

#include "cgi/CgiResponseMode.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "service/ServiceEventResult.hpp"
#include "utils.hpp"
#include "utils/ErrorPageProvider.hpp"

namespace webserv
{

CgiResponse::CgiResponse(
	const Logger &logger
) : _mode(CgiResponseMode::DOCUMENT),
		_ContentType("text/html"),
		_LocalLocation(""),
		_ClientLocation(""),
		_StatusCode("200"),
		_ReasonPhrase("OK"),
		_IsResponseHeaderParsed(false),
		_IsParseCompleted(false),
		logger(logger)
{
}

CgiResponse::~CgiResponse()
{
}

CgiResponse::CgiResponse(
	const CgiResponse &other
) : _mode(other._mode),
		_ContentType(other._ContentType),
		_LocalLocation(other._LocalLocation),
		_ClientLocation(other._ClientLocation),
		_StatusCode(other._StatusCode),
		_ReasonPhrase(other._ReasonPhrase),
		_ProtocolFieldMap(other._ProtocolFieldMap),
		_ExtensionFieldMap(other._ExtensionFieldMap),
		_responseBody(other._responseBody)
{
}

CgiResponse &CgiResponse::operator=(const CgiResponse &other)
{
	if (this == &other) {
		return *this;
	}

	_mode = other._mode;
	_ContentType = other._ContentType;
	_LocalLocation = other._LocalLocation;
	_ClientLocation = other._ClientLocation;
	_StatusCode = other._StatusCode;
	_ReasonPhrase = other._ReasonPhrase;
	_ProtocolFieldMap = other._ProtocolFieldMap;
	_ExtensionFieldMap = other._ExtensionFieldMap;
	_responseBody = other._responseBody;

	return *this;
}

std::vector<uint8_t> CgiResponse::generateResponsePacket() const
{
	HttpResponse httpResponse;
	httpResponse.setVersion("HTTP/1.1");
	httpResponse.setStatusCode(_StatusCode);
	httpResponse.setReasonPhrase(_ReasonPhrase);
	httpResponse.setHeaders(_ProtocolFieldMap);

	if (_ContentType.empty()) {
		httpResponse.getHeaders().addValue("Content-Type", _ContentType);
	}

	httpResponse.setBody(_responseBody);
	return httpResponse.generateResponsePacket();
}

bool CgiResponse::pushResponseRaw(
	const std::vector<uint8_t> &responseRaw
)
{
	_UnparsedResponseRaw.insert(
		_UnparsedResponseRaw.end(),
		responseRaw.begin(),
		responseRaw.end()
	);

	std::vector<uint8_t> *responseRawLine = utils::pickLine(_UnparsedResponseRaw);
	if (responseRawLine == NULL) {
		return true;
	}

	if (_IsResponseHeaderParsed == false) {
		_IsResponseHeaderParsed = parseResponseHeader(*responseRawLine);
		delete responseRawLine;
		if (_IsResponseHeaderParsed == false) {
			_IsParseCompleted = true;
			return false;
		}
		responseRawLine = utils::pickLine(_UnparsedResponseRaw);
		if (responseRawLine == NULL) {
			return true;
		}
	}

	delete responseRawLine;
	return true;
}

std::vector<uint8_t> CgiResponse::getUnparsedResponseRaw() const
{
	return _UnparsedResponseRaw;
}

bool CgiResponse::parseResponseHeader(
	const std::vector<uint8_t> &responseRawLine
)
{
	std::pair<std::string, std::string> nameValue = utils::splitNameValue(responseRawLine, ':');
	if (nameValue.first.empty()) {
		C_WARN("nameValue.first was empty");
		return false;
	}
	this->_ProtocolFieldMap.addValue(nameValue.first, nameValue.second);

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

const std::string &CgiResponse::getLocalLocation() const
{
	return this->_LocalLocation;
}

const std::string &CgiResponse::getClientLocation() const
{
	return this->_ClientLocation;
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

const std::vector<uint8_t> &CgiResponse::getResponseBody() const
{
	return this->_responseBody;
}

}	 // namespace webserv
