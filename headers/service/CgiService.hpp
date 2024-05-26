#pragma once

#include <unistd.h>

#include <EnvManager.hpp>
#include <cgi/CgiExecuter.hpp>
#include <cgi/CgiHandler.hpp>
#include <service/RequestedFileInfo.hpp>
#include <service/ServiceBase.hpp>

namespace webserv
{

class CgiService : public ServiceBase
{
 private:
	pid_t _pid;
	CgiExecuter *_cgiExecuter;
	CgiHandler *_cgiHandler;

 public:
	CgiService(
		const HttpRequest &request,
		const RequestedFileInfo &requestedFileInfo,
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
