#pragma once

#include <service/ServiceBase.hpp>
#include <types.hpp>

#include "./CgiHandler.hpp"

namespace webserv
{

class CgiExecuter
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
		char **argv,
		char **envp
	);

	CgiExecuter(const CgiExecuter &src);
	CgiExecuter &operator=(const CgiExecuter &src);

 public:
	CgiExecuter(
		const std::vector<uint8_t> &requestBody,
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
