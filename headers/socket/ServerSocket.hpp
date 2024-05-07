#pragma once

#include <config/ListenConfig.hpp>
#include <config/ServerRunningConfig.hpp>

#include "../Logger.hpp"
#include "./Socket.hpp"

namespace webserv
{

class ServerSocket : public Socket
{
 private:
	Logger logger;
	const ServerRunningConfigListType &_listenConfigList;
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

	virtual SockEventResultType onEventGot(
		short revents,
		std::vector<Socket *> &sockets
	);
};

}	 // namespace webserv
