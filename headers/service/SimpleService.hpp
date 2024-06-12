#pragma once

#include <poll.h>

#include <Logger.hpp>
#include <config/HttpRouteConfig.hpp>
#include <http/HttpRequest.hpp>
#include <http/HttpResponse.hpp>
#include <utils/ErrorPageProvider.hpp>

#include "./ServiceBase.hpp"
#include "./ServiceEventResult.hpp"

namespace webserv
{

class SimpleService : public ServiceBase
{
 public:
	SimpleService(
		const HttpRequest &request,
		const HttpResponse &response,
		const Logger &logger
	);
	virtual ~SimpleService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual ServiceEventResultType onEventGot(
		short revents
	);
};

}	 // namespace webserv
