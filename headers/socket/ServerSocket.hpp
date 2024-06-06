#pragma once

#include <config/ListenConfig.hpp>
#include <config/ServerRunningConfig.hpp>
#include <poll/Pollable.hpp>

#include "../Logger.hpp"

namespace webserv
{

class ServerSocket : public Pollable
{
 private:
	Logger logger;
	ServerRunningConfigListType _listenConfigList;
	ServerSocket(
		int fd,
		const Logger &logger,
		const ServerRunningConfigListType &listenConfigList
	);

 public:
	virtual ~ServerSocket();

	/**
	 * @brief 指定のポートに紐づいたServerSocketを作成する
	 *
	 * @param listenConfigList 指定のポートに紐づいたServerSocketの設定
	 * @return ServerSocket* 作成されたServerSocketのポインタ
	 */
	static ServerSocket *createServerSocket(
		const ServerRunningConfigListType &listenConfigList,
		uint16_t port,
		const Logger &logger
	);

	virtual void setToPollFd(
		struct pollfd &pollFd
	) const;

	virtual PollEventResultType onEventGot(
		int fd,
		short revents,
		std::vector<Pollable *> &pollableList
	);
};

}	 // namespace webserv
