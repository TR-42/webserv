#pragma once

#include <service/ServiceBase.hpp>
#include <socket/Socket.hpp>

#include "./CgiHandlerService.hpp"

namespace webserv
{

class CgiExecuterService : public ServiceBase
{
 private:
	int _fdWriteToCgi;
	int _fdReadFromCgi;
	bool _isReaderInstanceCreated;
	pid_t _pid;

	void _childProcessFunc(
		std::vector<Socket *> &sockets,
		int fdReadFromParent,
		int fdWriteToParent,
		const std::string &cgiPath,
		char **envp
	);

 public:
	CgiExecuterService(
		const HttpRequest &request,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		std::vector<Socket *> &sockets
	);
	virtual ~CgiExecuterService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual ServiceEventResultType onEventGot(
		short revents
	);

	virtual bool isWriterInstance() const;

	CgiHandlerService *createCgiHandlerService();
};

}	 // namespace webserv
