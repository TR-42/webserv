#include <unistd.h>

#include <poll/Pollable.hpp>
#include <stdexcept>
#include <utils/UUIDv7.hpp>

namespace webserv
{

Pollable::Pollable(
	int fd
) : _fd(fd),
		_uuid(utils::UUIDv7()),
		_isDisposingFromChildProcess(false)
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
	struct pollfd &pollFd,
	const struct timespec &now
) const
{
	(void)now;
	pollFd.fd = this->_fd;
	pollFd.events = POLLIN | POLLOUT;
	pollFd.revents = 0;
}

utils::UUID Pollable::getUUID() const
{
	return this->_uuid;
}

bool Pollable::isFdSame(int fd) const
{
	return this->_fd == fd;
}

bool Pollable::isDisposingFromChildProcess() const
{
	return this->_isDisposingFromChildProcess;
}

void Pollable::setIsDisposingFromChildProcess(
	bool value
)
{
	this->_isDisposingFromChildProcess = value;
}

}	 // namespace webserv
