#pragma once

#include <cgi/CgiExecuterService.hpp>
#include <cgi/CgiHandlerService.hpp>
#include <service/ServiceBase.hpp>

namespace webserv
{

class CgiService : public ServiceBase
{
 private:
	CgiExecuterService *_cgiExecuterService;
	CgiHandlerService *_cgiHandlerService;

 public:
	CgiService(
		const HttpRequest &request,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		std::vector<Pollable *> &pollableList
	);
	~CgiService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual ServiceEventResultType onEventGot(
		short revents
	);
};

};	// namespace webserv
