#pragma once

#include <poll.h>

#include <utils/UUID.hpp>
#include <vector>

#include "./PollEventResult.hpp"

namespace webserv
{

typedef struct pollfd Pollfd;

class Pollable
{
 private:
	int _fd;
	utils::UUID _uuid;
	bool _isDisposingFromChildProcess;

	// FDを扱う関係で、コピーは許可しない
	Pollable *operator=(const Pollable &);
	Pollable(const Pollable &);

 protected:
	Pollable(int fd);
	int getFD() const;

 public:
	virtual ~Pollable();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual PollEventResultType onEventGot(
		int fd,
		short revents,
		std::vector<Pollable *> &pollableList
	) = 0;

	utils::UUID getUUID() const;

	bool isFdSame(int fd) const;

	bool isDisposingFromChildProcess() const;
	void setIsDisposingFromChildProcess(bool value);
};

}	 // namespace webserv
