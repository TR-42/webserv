#pragma once

#include <Logger.hpp>
#include <ctime>
#include <stdexcept>

#define DEFAULT_REQUEST_TIMEOUT_MS (100)
#ifdef DEBUG
#define SOCKET_TIMEOUT_MS (10 * 1000)
#else
#define SOCKET_TIMEOUT_MS (1 * 1000)
#endif

namespace webserv
{

class TimeoutChecker
{
 private:
	TimeoutChecker(const TimeoutChecker &)
	{
		throw std::runtime_error("TimeoutChecker should not be copied");
	}
	TimeoutChecker &operator=(const TimeoutChecker &)
	{
		throw std::runtime_error("TimeoutChecker should not be copied");
	}

	struct timespec _start;
	size_t _timeout_ms;
	Logger logger;

 public:
	TimeoutChecker(
		const timespec &start,
		const Logger &logger,
		size_t timeout_ms = DEFAULT_REQUEST_TIMEOUT_MS
	);
	virtual ~TimeoutChecker();

	void setTimeoutMs(size_t timeout_ms);
	size_t getElapsedMs(const struct timespec &now) const;

	bool isTimeouted(const struct timespec &now) const;

	bool isConnectionTimeouted(const struct timespec &now) const;
};

}	 // namespace webserv
