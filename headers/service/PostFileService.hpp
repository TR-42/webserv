#pragma once

#include <poll.h>

#include <Logger.hpp>
#include <http/HttpRequest.hpp>
#include <http/HttpResponse.hpp>
#include <utils/ErrorPageProvider.hpp>

#include "./ServiceBase.hpp"
#include "./ServiceEventResult.hpp"

namespace webserv
{

class PostFileService : public ServiceBase
{
 protected:
	int _fd;
	size_t _writtenSize;

 public:
	PostFileService(
		const HttpRequest &request,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger
	);
	virtual ~PostFileService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual ServiceEventResultType onEventGot(
		short revents
	);
};

}	 // namespace webserv
