#pragma once

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

 public:
	CgiExecuterService(
		const HttpRequest &request,
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger
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
