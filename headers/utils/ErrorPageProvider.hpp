#pragma once

#include <http/HttpResponse.hpp>
#include <map>
#include <string>

namespace webserv
{

namespace utils
{

class ErrorPageProvider
{
 public:
	ErrorPageProvider();
	~ErrorPageProvider();
	HttpResponse badRequest() const;
	HttpResponse permissionDenied() const;
	HttpResponse notFound() const;
	HttpResponse movedPermanently() const;
	HttpResponse found() const;
	HttpResponse internalServerError() const;
	HttpResponse notImplemented() const;
	HttpResponse serviceUnavailable() const;
	HttpResponse gatewayTimeout() const;
	HttpResponse httpVersionNotSupported() const;
	HttpResponse getErrorPage(const std::string &path) const;
	HttpResponse getErrorPage(int statusCode) const;

	static const int BAD_REQUEST = 400;
	static const int PERMISSION_DENIED = 403;
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

}	 // namespace utils

}	 // namespace webserv
