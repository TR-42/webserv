#pragma once

#include <service/ServiceBase.hpp>
#include <types.hpp>

#include "./CgiHandler.hpp"

namespace webserv
{

class CgiExecuter
{
 private:
	const std::vector<uint8_t> &_requestBody;
	int _fdWriteToCgi;
	pid_t _pid;
	Logger logger;
	size_t _writtenCount;

	void _childProcessFunc(
		std::string workingDir,
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
		const std::string &workingDir,
		const Logger &logger,
		int fdWriteToCgi,
		int fdReadFromParent,
		int fdReadFromCgi,
		int fdWriteToParent
	);
	virtual ~CgiExecuter();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual PollEventResultType onEventGot(
		short revents
	);

	pid_t getPid() const;
	bool isWriteToCgiCompleted() const;
};

}	 // namespace webserv
