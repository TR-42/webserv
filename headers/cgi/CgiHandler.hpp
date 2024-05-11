#pragma once

#include <Logger.hpp>
#include <poll/Pollable.hpp>
#include <utils/ErrorPageProvider.hpp>

namespace webserv
{

class CgiHandler : public Pollable
{
 private:
	Logger logger;

 public:
	CgiHandler(
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		int fdReadFromCgi
	);
	virtual ~CgiHandler();

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
