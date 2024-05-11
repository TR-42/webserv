#pragma once

#include <Logger.hpp>
#include <poll/Pollable.hpp>
#include <utils/ErrorPageProvider.hpp>

namespace webserv
{

class CgiHandlerService : public Pollable
{
 private:
	Logger logger;

 public:
	CgiHandlerService(
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		int fdReadFromCgi
	);
	virtual ~CgiHandlerService();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual PollEventResultType onEventGot(
		short revents,
		std::vector<Pollable *> &pollableList
	);

	// TODO: Redirect対応
};

}	 // namespace webserv
