#pragma once

#include <config/HttpRouteConfig.hpp>
#include <service/RequestedFileInfo.hpp>

#include "ServiceBase.hpp"

namespace webserv
{

class DeleteFileService : public ServiceBase
{
 public:
	DeleteFileService(
		const HttpRequest &request,
		const RequestedFileInfo &requestedFileInfo,
		const webserv::utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger
	);
	virtual ~DeleteFileService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual ServiceEventResultType onEventGot(
		short revents
	);
};

}	 // namespace webserv
