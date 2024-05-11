#pragma once

#include <poll/Pollable.hpp>
#include <service/ServiceBase.hpp>

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

	virtual ServiceEventResultType onEventGot(
		short revents
	);

	virtual bool isWriterInstance() const;

	CgiHandlerService *createCgiHandlerService();
};

}	 // namespace webserv
