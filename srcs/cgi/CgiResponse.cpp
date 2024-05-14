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
) : _fd(-1),
		_mode(CgiResponseMode::DOCUMENT),
		_ContentType("text/html"),
		_LocalLocation(""),
		_ClientLocation(""),
		_StatusCode("200"),
		_ReasonPhrase("OK")
{
}

CgiResponse::~CgiResponse()
{
	if (_fd != -1)
		close(_fd);
}

CgiResponse::CgiResponse(const CgiResponse &other)
		: _fd(other._fd), _errorPageProvider(other._errorPageProvider), _mode(other._mode), _ContentType(other._ContentType), _LocalLocation(other._LocalLocation), _ClientLocation(other._ClientLocation), _StatusCode(other._StatusCode), _ReasonPhrase(other._ReasonPhrase), _ProtocolFieldMap(other._ProtocolFieldMap), _ExtensionFieldMap(other._ExtensionFieldMap), _responseBody(other._responseBody)
{
}

CgiResponse &CgiResponse::operator=(const CgiResponse &other)
{
	if (this != &other) {
		if (_fd != -1)
			close(_fd);
		_fd = other._fd;
		_errorPageProvider = other._errorPageProvider;
		_mode = other._mode;
		_ContentType = other._ContentType;
		_LocalLocation = other._LocalLocation;
		_ClientLocation = other._ClientLocation;
		_StatusCode = other._StatusCode;
		_ReasonPhrase = other._ReasonPhrase;
		_ProtocolFieldMap = other._ProtocolFieldMap;
		_ExtensionFieldMap = other._ExtensionFieldMap;
		_responseBody = other._responseBody;
	}
	return *this;
}

void CgiResponse::setResponseBody(const std::string &body)
{
	_responseBody = std::vector<uint8_t>(body.begin(), body.end());
}

std::vector<uint8_t> CgiResponse::generateResponsePacket() const
{
	// TODO: implement
}

}	 // namespace webserv
