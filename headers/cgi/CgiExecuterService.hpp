#pragma once

#include <poll/Pollable.hpp>
#include <service/ServiceBase.hpp>

#include "./CgiHandlerService.hpp"

namespace webserv
{

class CgiExecuterService : public Pollable
{
 private:
	int _fdWriteToCgi;
	pid_t _pid;
	CgiHandlerService *_cgiHandlerService;
	Logger logger;

	void _childProcessFunc(
		std::vector<Pollable *> &pollableList,
		int fdReadFromParent,
		int fdWriteToParent,
		const std::string &cgiPath,
		char **argv,
		char **envp
	);

 public:
	CgiExecuterService(
		const HttpRequest &request,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		std::vector<Pollable *> &pollableList
	);
	virtual ~CgiExecuterService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual PollEventResultType onEventGot(
		short revents,
		std::vector<Pollable *> &pollableList
	);

	CgiHandlerService *getCgiHandlerService() const;
};

}	 // namespace webserv
