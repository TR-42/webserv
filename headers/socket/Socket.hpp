#pragma once

#include <poll.h>

#include <vector>

#include "../utils/UUID.hpp"
#include "./SockEventResult.hpp"

namespace webserv
{

class Socket
{
 private:
	int _fd;
	utils::UUID _uuid;

	// FDを扱う関係で、コピーは許可しない
	Socket *operator=(const Socket &);
	Socket(const Socket &);

 protected:
	Socket(int fd);
	int getFD() const;

 public:
	virtual ~Socket();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual SockEventResultType onEventGot(
		short revents,
		std::vector<Socket *> &sockets
	) = 0;

	utils::UUID getUUID() const;
};

}	 // namespace webserv
