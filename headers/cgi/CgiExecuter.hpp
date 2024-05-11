#pragma once

#include <poll/Pollable.hpp>
#include <service/ServiceBase.hpp>

#include "./CgiHandler.hpp"

namespace webserv
{

class CgiExecuter : public Pollable
{
 private:
	int _fdWriteToCgi;
	pid_t _pid;
	CgiHandler *_cgiHandler;
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
	CgiExecuter(
		const HttpRequest &request,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		std::vector<Pollable *> &pollableList
	);
	virtual ~CgiExecuter();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual PollEventResultType onEventGot(
		short revents,
		std::vector<Pollable *> &pollableList
	);

	CgiHandler *getCgiHandler() const;
};

}	 // namespace webserv
