#include <http/HttpResponse.hpp>
#include <string>

#include "utils.hpp"

namespace webserv
{

namespace utils
{

class ErrorPageProvider
{
 public:
	ErrorPageProvider();
	~ErrorPageProvider();
	HttpResponse getErrorPage(int statusCode) const;
	HttpResponse badRequest() const;
	HttpResponse notFound() const;
	HttpResponse movedPermanently() const;
	HttpResponse found() const;
	HttpResponse internalServerError() const;
	HttpResponse notImplemented() const;
	HttpResponse serviceUnavailable() const;
	HttpResponse gatewayTimeout() const;
	HttpResponse httpVersionNotSupported() const;

	static const int BAD_REQUEST = 400;
	static const int NOT_FOUND = 404;
	static const int MOVED_PERMANENTLY = 301;
	static const int FOUND = 302;
	static const int INTERNAL_SERVER_ERROR = 500;
	static const int NOT_IMPLEMENTED = 501;
	static const int SERVICE_UNAVAILABLE = 503;
	static const int GATEWAY_TIMEOUT = 504;
	static const int HTTP_VERSION_NOT_SUPPORTED = 505;

 private:
	std::map<int, HttpResponse> _errorPages;
};

static HttpResponse createResponse(int statusCode, const std::string &reasonPhrase, const std::string &body)
{
	HttpResponse response;
	response.setStatusCode(to_string(statusCode));
	response.setReasonPhrase(reasonPhrase);
	response.setBody(body);

	std::vector<std::string> contentLengthHeader;
	ResponseHeaderMap headers;
	contentLengthHeader.push_back(to_string(body.size()));
	headers["Content-Length"] = contentLengthHeader;
	response.setHeaders(headers);
	return response;
}

static const HttpResponse defaultBadRequest = createResponse(
	ErrorPageProvider::BAD_REQUEST,
	"Bad Request",
	"400 Bad Request\n"
);

ErrorPageProvider::ErrorPageProvider()
{
	this->_errorPages[ErrorPageProvider::BAD_REQUEST] = defaultBadRequest;
	this->_errorPages[ErrorPageProvider::NOT_FOUND] = this->notFound();
	this->_errorPages[ErrorPageProvider::MOVED_PERMANENTLY] = this->movedPermanently();
	this->_errorPages[ErrorPageProvider::FOUND] = this->found();
	this->_errorPages[ErrorPageProvider::INTERNAL_SERVER_ERROR] = this->internalServerError();
	this->_errorPages[ErrorPageProvider::NOT_IMPLEMENTED] = this->notImplemented();
	this->_errorPages[ErrorPageProvider::SERVICE_UNAVAILABLE] = this->serviceUnavailable();
	this->_errorPages[ErrorPageProvider::GATEWAY_TIMEOUT] = this->gatewayTimeout();
	this->_errorPages[ErrorPageProvider::HTTP_VERSION_NOT_SUPPORTED] = this->httpVersionNotSupported();
}

ErrorPageProvider::~ErrorPageProvider()
{
}

HttpResponse ErrorPageProvider::getErrorPage(int statusCode) const
{
	return this->_errorPages.at(statusCode);
}

HttpResponse ErrorPageProvider::badRequest() const
{
	return this->_errorPages.at(ErrorPageProvider::BAD_REQUEST);
}

HttpResponse ErrorPageProvider::notFound() const
{
	return this->_errorPages.at(ErrorPageProvider::NOT_FOUND);
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

}	 // namespace utils

}	 // namespace webserv
