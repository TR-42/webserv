#include <algorithm>
#include <iostream>
#include <socket/TimeoutChecker.hpp>

#define SEC_TO_MS(sec) ((sec) * 1000)
#define NSEC_TO_MS(nsec) ((nsec) / (1000 * 1000))

namespace webserv
{

TimeoutChecker::TimeoutChecker(
	const timespec &start,
	const Logger &logger,
	size_t timeout_ms
) : _start(start),
		logger(logger)
{
	this->setTimeoutMs(timeout_ms);
	CS_INFO()
		<< "TimeoutMs: " << std::dec << this->_timeout_ms
		<< std::endl;
}

TimeoutChecker::~TimeoutChecker()
{
}

void TimeoutChecker::setTimeoutMs(
	size_t timeout_ms
)
{
	this->_timeout_ms = std::min(timeout_ms, (size_t)SOCKET_TIMEOUT_MS);
	CS_INFO()
		<< "TimeoutMs: " << std::dec << this->_timeout_ms
		<< "(requested: " << std::dec << timeout_ms << ")"
		<< std::endl;
}

size_t TimeoutChecker::getElapsedMs(
	const struct timespec &now
) const
{
	return (size_t)(SEC_TO_MS(now.tv_sec - this->_start.tv_sec) + NSEC_TO_MS(now.tv_nsec - this->_start.tv_nsec));
}

bool TimeoutChecker::isTimeouted(
	const timespec &now
) const
{
	return this->_timeout_ms <= this->getElapsedMs(now);
}

bool TimeoutChecker::isConnectionTimeouted(
	const timespec &now
) const
{
	return SOCKET_TIMEOUT_MS <= this->getElapsedMs(now);
}

}	 // namespace webserv
