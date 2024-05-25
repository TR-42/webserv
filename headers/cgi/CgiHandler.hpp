#pragma once

#include <Logger.hpp>
#include <poll/Pollable.hpp>
#include <types.hpp>
#include <utils/ErrorPageProvider.hpp>

#include "./CgiResponse.hpp"

namespace webserv
{

class CgiHandler : public Pollable
{
 private:
	Logger logger;
	const utils::ErrorPageProvider &_errorPageProvider;
	bool _isAnyResponseReceived;
	CgiResponse _cgiResponse;
	CgiHandler **_cgiServiceCgiHandlerField;
	HttpResponse *_cgiServiceHttpResponseField;

	CgiHandler(const CgiHandler &src);
	CgiHandler &operator=(const CgiHandler &src);

 public:
	CgiHandler(
		const utils::ErrorPageProvider &errorPageProvider,
		const Logger &logger,
		int fdReadFromCgi,
		CgiHandler **_cgiServiceCgiHandlerField,
		HttpResponse *_cgiServiceHttpResponseField
	);
	virtual ~CgiHandler();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual PollEventResultType onEventGot(
		int fd,
		short revents,
		std::vector<Pollable *> &pollableList
	);

	void setDisposeRequested();

	// TODO: Redirect対応
};

}	 // namespace webserv
