#pragma once

#include "../Logger.hpp"
#include "./Socket.hpp"

namespace webserv
{

class ServerSocket : public Socket
{
 private:
	Logger logger;

 public:
	ServerSocket(
		int fd,
		const Logger &logger
	);
	virtual ~ServerSocket();

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual SockEventResultType onEventGot(
		short revents,
		std::vector<Socket *> &sockets
	);
};

}	 // namespace webserv
