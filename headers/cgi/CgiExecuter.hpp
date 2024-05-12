#pragma once

#include <poll/Pollable.hpp>
#include <service/ServiceBase.hpp>

#include "./CgiHandler.hpp"

namespace webserv
{

class CgiExecuter : public Pollable
{
 private:
	std::vector<uint8_t> _requestBody;
	int _fdWriteToCgi;
	pid_t _pid;
	Logger logger;
	size_t _writtenCount;

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
		const std::vector<uint8_t> &requestBody,
		const std::string &executablePath,
		char **argv,
		char **envp,
		const Logger &logger,
		int fdWriteToCgi,
		int fdReadFromParent,
		int fdReadFromCgi,
		int fdWriteToParent,
		std::vector<Pollable *> &pollableList
	);
	virtual ~CgiExecuter();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	PollEventResultType onEventGot(
		short revents
	);
	virtual PollEventResultType onEventGot(
		short revents,
		std::vector<Pollable *> &pollableList
	);

	pid_t getPid() const;
	bool isWriteToCgiCompleted() const;
};

}	 // namespace webserv
