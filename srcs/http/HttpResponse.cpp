#include <http/HttpResponse.hpp>

#include "http/HttpFieldMap.hpp"

namespace webserv
{

IMPL_GETTER_SETTER_NS(std::string, Version, HttpResponse::)
IMPL_GETTER_SETTER_NS(std::string, StatusCode, HttpResponse::)
IMPL_GETTER_SETTER_NS(std::string, ReasonPhrase, HttpResponse::)
IMPL_REF_NO_CONST_GETTER_SETTER_NS(HttpFieldMap, Headers, HttpResponse::)
IMPL_REF_NO_CONST_GETTER_SETTER_NS(std::vector<uint8_t>, Body, HttpResponse::)

HttpResponse::HttpResponse(
) : _Version("HTTP/1.1"),
		_StatusCode("200"),
		_ReasonPhrase("OK"),
		_Headers(),
		_Body()
{
}

HttpResponse::HttpResponse(
	const HttpResponse &src
) : _Version(src._Version),
		_StatusCode(src._StatusCode),
		_ReasonPhrase(src._ReasonPhrase),
		_Headers(src._Headers),
		_Body(src._Body)
{
}

HttpResponse &HttpResponse::operator=(const HttpResponse &src)
{
	if (this == &src) {
		return *this;
	}

	this->_Version = src._Version;
	this->_StatusCode = src._StatusCode;
	this->_ReasonPhrase = src._ReasonPhrase;
	this->_Headers = src._Headers;
	this->_Body = src._Body;

	return *this;
}

HttpResponse::~HttpResponse()
{
}

void HttpResponse::setBody(const std::string &body)
{
	this->_Body = std::vector<uint8_t>(body.begin(), body.end());
}

std::vector<uint8_t> HttpResponse::generateResponsePacket(
	bool withBody
) const
{
	std::vector<uint8_t> responsePacket;
	responsePacket.reserve(1024);

	responsePacket.insert(responsePacket.end(), this->_Version.begin(), this->_Version.end());
	responsePacket.push_back(' ');
	responsePacket.insert(responsePacket.end(), this->_StatusCode.begin(), this->_StatusCode.end());
	responsePacket.push_back(' ');
	responsePacket.insert(responsePacket.end(), this->_ReasonPhrase.begin(), this->_ReasonPhrase.end());
	responsePacket.push_back('\r');
	responsePacket.push_back('\n');

	this->_Headers.appendToVector(responsePacket, this->_Body);
	responsePacket.push_back('\r');
	responsePacket.push_back('\n');

	if (withBody) {
		responsePacket.insert(responsePacket.end(), this->_Body.begin(), this->_Body.end());
	}

	return responsePacket;
}

}	 // namespace webserv
