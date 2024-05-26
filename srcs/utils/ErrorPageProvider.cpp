#include "utils/ErrorPageProvider.hpp"

#include <fstream>
#include <http/HttpResponse.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utils/stoul.hpp>
#include <utils/to_string.hpp>

#include "http/HttpFieldMap.hpp"

namespace webserv
{

namespace utils
{

const int ErrorPageProvider::NO_CONTENT;
const int ErrorPageProvider::BAD_REQUEST;
const int ErrorPageProvider::PERMISSION_DENIED;
const int ErrorPageProvider::NOT_FOUND;
const int ErrorPageProvider::METHOD_NOT_ALLOWED;
const int ErrorPageProvider::MOVED_PERMANENTLY;
const int ErrorPageProvider::FOUND;
const int ErrorPageProvider::INTERNAL_SERVER_ERROR;
const int ErrorPageProvider::NOT_IMPLEMENTED;
const int ErrorPageProvider::SERVICE_UNAVAILABLE;
const int ErrorPageProvider::GATEWAY_TIMEOUT;
const int ErrorPageProvider::HTTP_VERSION_NOT_SUPPORTED;

static HttpResponse createResponse(int statusCode, const std::string &reasonPhrase, const std::string &body)
{
	HttpResponse response;
	response.setVersion("HTTP/1.1");
	response.setStatusCode(to_string(statusCode));
	response.setReasonPhrase(reasonPhrase);
	response.setBody(body);

	HttpFieldMap headers;
	headers.addValue("Content-Length", to_string(body.size()));
	response.setHeaders(headers);
	return response;
}

static const HttpResponse defaultNoContent = createResponse(
	ErrorPageProvider::NO_CONTENT,
	"No Content",
	"204 No Content\n"
);

static const HttpResponse defaultBadRequest = createResponse(
	ErrorPageProvider::BAD_REQUEST,
	"Bad Request",
	"400 Bad Request\n"
);

static const HttpResponse defaultPermissionDenied = createResponse(
	ErrorPageProvider::PERMISSION_DENIED,
	"Permission Denied",
	"403 Permission Denied\n"
);

static const HttpResponse defaultNotFound = createResponse(
	ErrorPageProvider::NOT_FOUND,
	"Not Found",
	"404 Not Found\n"
);

static const HttpResponse defaultMethodNotAllowed = createResponse(
	ErrorPageProvider::METHOD_NOT_ALLOWED,
	"Method Not Allowed",
	"405 Method Not Allowed\n"
);

static const HttpResponse defaultMovedPermanently = createResponse(
	ErrorPageProvider::MOVED_PERMANENTLY,
	"Moved Permanently",
	"301 Moved Permanently\n"
);

static const HttpResponse defaultFound = createResponse(
	ErrorPageProvider::FOUND,
	"Found",
	"302 Found\n"
);

static const HttpResponse defaultInternalServerError = createResponse(
	ErrorPageProvider::INTERNAL_SERVER_ERROR,
	"Internal Server Error",
	"500 Internal Server Error\n"
);

static const HttpResponse defaultNotImplemented = createResponse(
	ErrorPageProvider::NOT_IMPLEMENTED,
	"Not Implemented",
	"501 Not Implemented\n"
);

static const HttpResponse defaultServiceUnavailable = createResponse(
	ErrorPageProvider::SERVICE_UNAVAILABLE,
	"Service Unavailable",
	"503 Service Unavailable\n"
);

static const HttpResponse defaultGatewayTimeout = createResponse(
	ErrorPageProvider::GATEWAY_TIMEOUT,
	"Gateway Timeout",
	"504 Gateway Timeout\n"
);

static const HttpResponse defaultHttpVersionNotSupported = createResponse(
	ErrorPageProvider::HTTP_VERSION_NOT_SUPPORTED,
	"HTTP Version Not Supported",
	"505 HTTP Version Not Supported\n"
);

ErrorPageProvider::ErrorPageProvider()
{
	this->_errorPages[ErrorPageProvider::NO_CONTENT] = defaultNoContent;
	this->_errorPages[ErrorPageProvider::BAD_REQUEST] = defaultBadRequest;
	this->_errorPages[ErrorPageProvider::PERMISSION_DENIED] = defaultPermissionDenied;
	this->_errorPages[ErrorPageProvider::NOT_FOUND] = defaultNotFound;
	this->_errorPages[ErrorPageProvider::MOVED_PERMANENTLY] = defaultMovedPermanently;
	this->_errorPages[ErrorPageProvider::FOUND] = defaultFound;
	this->_errorPages[ErrorPageProvider::INTERNAL_SERVER_ERROR] = defaultInternalServerError;
	this->_errorPages[ErrorPageProvider::NOT_IMPLEMENTED] = defaultNotImplemented;
	this->_errorPages[ErrorPageProvider::SERVICE_UNAVAILABLE] = defaultServiceUnavailable;
	this->_errorPages[ErrorPageProvider::GATEWAY_TIMEOUT] = defaultGatewayTimeout;
	this->_errorPages[ErrorPageProvider::HTTP_VERSION_NOT_SUPPORTED] = defaultHttpVersionNotSupported;
}

ErrorPageProvider::ErrorPageProvider(
	const ErrorPageProvider &src
) : _errorPages(src._errorPages)
{
}

ErrorPageProvider &ErrorPageProvider::operator=(
	const ErrorPageProvider &src
)
{
	if (this == &src) {
		return *this;
	}

	this->_errorPages = src._errorPages;

	return *this;
}

void ErrorPageProvider::setErrorPageFromFile(
	int statusCode,
	const std::string &path
)
{
	std::string content;

	if (path.empty()) {
		throw std::runtime_error("ErrorPageProvider::setErrorPageFromFile: path is empty");
		return;
	}

	std::stringstream buffer;
	{
		std::ifstream file;

		file.open(path.c_str(), std::ios::in);
		if (!file.is_open()) {
			throw std::runtime_error("ErrorPageProvider::setErrorPageFromFile: file not found");
			return;
		}

		buffer << file.rdbuf();
	}

	this->setErrorPageFromString(statusCode, buffer.str());
}

void ErrorPageProvider::setErrorPageFromString(
	int statusCode,
	const std::string &content
)
{
	// Reason Phrase再利用のため、既存を検索する (逆に、デフォルトに存在しない場合はセットできない)
	std::map<int, HttpResponse>::iterator it = this->_errorPages.find(statusCode);
	if (it == this->_errorPages.end()) {
		throw std::runtime_error("ErrorPageProvider::setErrorPageFromString: statusCode not found");
		return;
	}

	this->_errorPages[statusCode] = createResponse(statusCode, it->second.getReasonPhrase(), content);
}

ErrorPageProvider::~ErrorPageProvider()
{
}

HttpResponse ErrorPageProvider::getErrorPage(int statusCode) const
{
	return this->_errorPages.at(statusCode);
}

HttpResponse ErrorPageProvider::noContent() const
{
	return this->_errorPages.at(ErrorPageProvider::NO_CONTENT);
}

HttpResponse ErrorPageProvider::badRequest() const
{
	return this->_errorPages.at(ErrorPageProvider::BAD_REQUEST);
}

HttpResponse ErrorPageProvider::permissionDenied() const
{
	return this->_errorPages.at(ErrorPageProvider::PERMISSION_DENIED);
}

HttpResponse ErrorPageProvider::notFound() const
{
	return this->_errorPages.at(ErrorPageProvider::NOT_FOUND);
}

HttpResponse ErrorPageProvider::methodNotAllowed() const
{
	return this->_errorPages.at(ErrorPageProvider::METHOD_NOT_ALLOWED);
}

HttpResponse ErrorPageProvider::movedPermanently() const
{
	return this->_errorPages.at(ErrorPageProvider::MOVED_PERMANENTLY);
}

HttpResponse ErrorPageProvider::found() const
{
	return this->_errorPages.at(ErrorPageProvider::FOUND);
}

HttpResponse ErrorPageProvider::internalServerError() const
{
	return this->_errorPages.at(ErrorPageProvider::INTERNAL_SERVER_ERROR);
}

HttpResponse ErrorPageProvider::notImplemented() const
{
	return this->_errorPages.at(ErrorPageProvider::NOT_IMPLEMENTED);
}

HttpResponse ErrorPageProvider::serviceUnavailable() const
{
	return this->_errorPages.at(ErrorPageProvider::SERVICE_UNAVAILABLE);
}

HttpResponse ErrorPageProvider::gatewayTimeout() const
{
	return this->_errorPages.at(ErrorPageProvider::GATEWAY_TIMEOUT);
}

HttpResponse ErrorPageProvider::httpVersionNotSupported() const
{
	return this->_errorPages.at(ErrorPageProvider::HTTP_VERSION_NOT_SUPPORTED);
}

HttpResponse ErrorPageProvider::getErrorPage(const std::string &statusCode) const
{
	unsigned long statusCodeUL = 0;
	if (!utils::stoul(statusCode, statusCodeUL)) {
		return _errorPages.at(ErrorPageProvider::NOT_FOUND);
	}
	std::map<int, HttpResponse>::const_iterator it = this->_errorPages.find(statusCodeUL);
	if (it == this->_errorPages.end()) {
		return _errorPages.at(ErrorPageProvider::NOT_FOUND);
	}
	return it->second;
}

}	 // namespace utils

}	 // namespace webserv
