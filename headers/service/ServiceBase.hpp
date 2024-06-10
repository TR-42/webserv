#pragma once

#include <poll.h>

#include <Logger.hpp>
#include <http/HttpRequest.hpp>
#include <http/HttpResponse.hpp>

#include "./ServiceEventResult.hpp"

namespace webserv
{

class ServiceBase
{
 private:
	ServiceBase(
		const ServiceBase &src
	) : _request(src._request),
			logger(src.logger)
	{
		throw std::runtime_error("ServiceBase copy constructor is not allowed");
	}
	ServiceBase &operator=(
		const ServiceBase &
	)
	{
		throw std::runtime_error("ServiceBase copy assignment operator is not allowed");
	}

 protected:
	const HttpRequest &_request;
	HttpResponse _response;
	const Logger &logger;
	bool _isDisposingFromChildProcess;

 public:
	ServiceBase(
		const HttpRequest &request,
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
	inline const utils::ErrorPageProvider &getErrorPageProvider() const
	{
		return this->_request.getServerRunningConfig().getErrorPageProvider();
	}
};

}	 // namespace webserv
