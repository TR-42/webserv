#pragma once

#include <poll.h>

#include <Logger.hpp>
#include <http/HttpRequest.hpp>
#include <http/HttpResponse.hpp>
#include <utils/ErrorPageProvider.hpp>

#include "./ServiceEventResult.hpp"

namespace webserv
{

class GetFileService
{
 protected:
	const HttpRequest &_request;
	HttpResponse _response;
	const utils::ErrorPageProvider &_errorPageProvider;
	const Logger &_logger;
	int _isDirectory;

 public:
	GetFileService(
		const HttpRequest &request,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger
	);
	virtual ~GetFileService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual ServiceEventResultType onEventGot(
		short revents
	);

	const HttpResponse &getResponse() const;
};

}	 // namespace webserv
