#pragma once

#include <service/ServiceBase.hpp>

namespace webserv
{

class CgiHandlerService : public ServiceBase
{
 private:
	int _fdReadFromCgi;

 public:
	CgiHandlerService(
		const HttpRequest &request,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		int fdReadFromCgi
	);
	virtual ~CgiHandlerService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual ServiceEventResultType onEventGot(
		short revents
	);

	// TODO: Redirect対応
};

}	 // namespace webserv
