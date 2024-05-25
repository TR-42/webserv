#pragma once

#include <poll.h>

#include <stdexcept>
#include <utils/UUID.hpp>
#include <vector>

#include "../Logger.hpp"
#include "./Pollable.hpp"

namespace webserv
{

class Poll
{
 private:
	typedef struct pollfd Pollfd;

	std::vector<Pollable *> _PollableList;
	std::vector<Pollfd> _PollFdList;

	Logger logger;

	void _onPollableDisposeRequested(
		size_t index,
		utils::UUID pollableUuid
	);

	Poll(const Poll &)
	{
		throw std::runtime_error("Poll copy constructor is not allowed");
	}
	Poll &operator=(const Poll &)
	{
		throw std::runtime_error("Poll copy assignment operator is not allowed");
	}

 public:
	Poll(
		const std::vector<Pollable *> &initialPollableList,
		const Logger &logger
	);
	virtual ~Poll();

	/**
	 * @brief イベントループ処理
	 *
	 * @return true 処理に成功 (次の処理を継続できる)
	 * @return false 処理に失敗し、可能な限りすぐに終了する必要がある
	 */
	bool loop();
};

}	 // namespace webserv
