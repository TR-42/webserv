#pragma once

#include <poll.h>

#include <Logger.hpp>
#include <http/HttpRequest.hpp>
#include <http/HttpResponse.hpp>
#include <utils/ErrorPageProvider.hpp>

#include "./ServiceEventResult.hpp"

namespace webserv
{

class ServiceBase
{
 protected:
	const HttpRequest &_request;
	HttpResponse _response;
	const utils::ErrorPageProvider &_errorPageProvider;
	const Logger &logger;
	bool _isDisposingFromChildProcess;

 public:
	ServiceBase(
		const HttpRequest &request,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger
	);
	virtual ~ServiceBase();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const = 0;

	virtual ServiceEventResultType onEventGot(
		short revents
	) = 0;

	const HttpResponse &getResponse() const;

	bool isDisposingFromChildProcess() const;
	void setIsDisposingFromChildProcess(bool value);
};

}	 // namespace webserv
