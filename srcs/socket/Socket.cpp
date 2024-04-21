#include <unistd.h>

#include <socket/Socket.hpp>
#include <stdexcept>
#include <utils/UUIDv7.hpp>

namespace webserv
{

Socket::Socket(
	int fd
) : _fd(fd),
		_uuid(utils::UUIDv7())
{
	if (fd < 0) {
		throw std::invalid_argument("Invalid FD");
	}
}

Socket::~Socket()
{
	if (0 <= this->_fd) {
		close(this->_fd);
	}
}

int Socket::getFD() const
{
	return this->_fd;
}

void Socket::setToPollFd(
	struct pollfd &pollFd
) const
{
	pollFd.fd = this->_fd;
	pollFd.events = POLLIN | POLLOUT;
	pollFd.revents = 0;
}

utils::UUID Socket::getUUID() const
{
	return this->_uuid;
}

#pragma region invalid operation
Socket::Socket(
	const Socket &src
) : _fd(src._fd),
		_uuid(src._uuid)
{
	*this = src;
}

Socket *Socket::operator=(const Socket &src)
{
	(void)src;
	throw std::logic_error("Not Allowed Operation");
	return NULL;
}
#pragma endregion

}	 // namespace webserv
