#pragma once

#include <poll.h>

#include <vector>

#include "../Logger.hpp"
#include "./Socket.hpp"

namespace webserv
{

class Poll
{
 private:
	typedef struct pollfd Pollfd;

	std::vector<Socket *> _SocketList;
	std::vector<Pollfd> _PollFdList;

	Logger logger;

	void _onSocketDisposeRequested(
		size_t index,
		utils::UUID socketUuid
	);

 public:
	Poll(
		const std::vector<Socket *> &initialSocketList,
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
