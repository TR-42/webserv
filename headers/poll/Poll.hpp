#pragma once

#include <poll.h>

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

 public:
	Poll(
		const std::vector<Pollable *> &initialPollableList,
		const Logger &logger
	);
	~Poll();

	/**
	 * @brief イベントループ処理
	 *
	 * @return true 処理に成功 (次の処理を継続できる)
	 * @return false 処理に失敗し、可能な限りすぐに終了する必要がある
	 */
	bool loop();
};

}	 // namespace webserv
