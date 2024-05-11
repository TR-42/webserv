#include <unistd.h>

#include <poll/Pollable.hpp>
#include <stdexcept>
#include <utils/UUIDv7.hpp>

namespace webserv
{

Pollable::Pollable(
	int fd
) : _fd(fd),
		_uuid(utils::UUIDv7())
{
	if (fd < 0) {
		throw std::invalid_argument("Invalid FD");
	}
}

Pollable::~Pollable()
{
	if (0 <= this->_fd) {
		close(this->_fd);
	}
}

int Pollable::getFD() const
{
	return this->_fd;
}

void Pollable::setToPollFd(
	struct pollfd &pollFd
) const
{
	pollFd.fd = this->_fd;
	pollFd.events = POLLIN | POLLOUT;
	pollFd.revents = 0;
}

utils::UUID Pollable::getUUID() const
{
	return this->_uuid;
}

#pragma region invalid operation
Pollable::Pollable(
	const Pollable &src
) : _fd(src._fd),
		_uuid(src._uuid)
{
	*this = src;
}

Pollable *Pollable::operator=(const Pollable &src)
{
	(void)src;
	throw std::logic_error("Not Allowed Operation");
	return NULL;
}
#pragma endregion

}	 // namespace webserv
