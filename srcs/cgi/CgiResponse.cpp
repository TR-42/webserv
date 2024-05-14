#include "cgi/CgiResponse.hpp"

#include <unistd.h>

#include "cgi/CgiResponseMode.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "service/ServiceEventResult.hpp"
#include "utils/ErrorPageProvider.hpp"

namespace webserv
{

CgiResponse::CgiResponse(
) : _mode(CgiResponseMode::DOCUMENT),
		_ContentType("text/html"),
		_LocalLocation(""),
		_ClientLocation(""),
		_StatusCode("200"),
		_ReasonPhrase("OK")
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
