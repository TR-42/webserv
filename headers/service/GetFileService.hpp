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

class GetFileService : public ServiceBase
{
 protected:
	bool _isDirectory;
	int _fd;

 public:
	GetFileService(
		const HttpRequest &request,
		const HttpRouteConfig &routeConfig,
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

	std::string generateFileList(const std::string &path, const std::string &requestPath);
};

}	 // namespace webserv
