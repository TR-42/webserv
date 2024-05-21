#pragma once

#include <unistd.h>

#include <EnvManager.hpp>
#include <cgi/CgiExecuter.hpp>
#include <cgi/CgiHandler.hpp>
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
		const std::string &cgiPath,
		const utils::ErrorPageProvider &errorPageProvider,
		const env::EnvManager &envPreset,
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
