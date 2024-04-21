#pragma once

#include <poll.h>

#include <Logger.hpp>
#include <http/HttpRequest.hpp>
#include <http/HttpResponse.hpp>
#include <utils/ErrorPageProvider.hpp>

#include "./ServiceEventResult.hpp"

namespace webserv
{

class SimpleService
{
 protected:
	const HttpRequest &_request;
	HttpResponse _response;
	const utils::ErrorPageProvider &_errorPageProvider;
	const Logger &_logger;

 public:
	SimpleService(
		const HttpRequest &request,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger
	);
	virtual ~SimpleService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual ServiceEventResultType onEventGot(
		short revents
	);

	const HttpResponse &getResponse() const;
};

}	 // namespace webserv
