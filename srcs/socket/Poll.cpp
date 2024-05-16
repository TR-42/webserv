#include <iostream>
#include <socket/Poll.hpp>
#include <utils.hpp>

#define POLL_TIMEOUT 1000

namespace webserv
{

Poll::Poll(
	const std::vector<Socket *> &initialSocketList,
	const Logger &logger
) : _SocketList(initialSocketList),
		logger(logger)
{
}

Poll::~Poll()
{
	const size_t socketCount = _SocketList.size();
	CS_DEBUG()
		<< "Poll instance destroying... "
		<< socketCount << " sockets left"
		<< std::endl;

	for (size_t i = 0; i < socketCount; i++) {
		delete this->_SocketList[i];
		// 念のためNULLを代入
		this->_SocketList[i] = NULL;
	}

	// 念のためクリア
	this->_SocketList.clear();
}

bool Poll::loop()
{
	std::vector<Socket *> _SocketListCopy(_SocketList);
	const size_t socketCount = _SocketListCopy.size();
	_PollFdList.resize(socketCount);
	for (size_t i = 0; i < socketCount; i++) {
		_SocketListCopy[i]->setToPollFd(_PollFdList[i]);
	}

	const int pollResult = poll(
		_PollFdList.data(),
		socketCount,
		POLL_TIMEOUT
	);
	if (pollResult < 0) {
		const char *errorStr = strerror(errno);
		CS_FATAL()
			<< "poll() failed: " << errorStr
			<< std::endl;
		return false;
	}

	size_t removeCount = 0;
	for (size_t i = 0; i < socketCount; i++) {
		utils::UUID socketUuid = _SocketListCopy[i]->getUUID();
		const SockEventResultType result = _SocketListCopy[i]->onEventGot(
			_PollFdList[i].revents,
			_SocketList
		);

		switch (result) {
			case SockEventResult::OK:
				// OKなら何もしない
				break;

			case SockEventResult::DISPOSE_REQUEST:
				this->_onSocketDisposeRequested(i - removeCount++, socketUuid);
				break;

			case SockEventResult::ERROR:
				CS_FATAL()
					<< "Error got from socket " << socketUuid
					<< std::endl;
				return false;

			default:
				CS_FATAL()
					<< "Unknown SockEventResultType: " << utils::to_string(result)
					<< " from socket " << socketUuid
					<< std::endl;
				return false;
		}
	}

	return true;
}

void Poll::_onSocketDisposeRequested(
	size_t index,
	utils::UUID socketUuid
)
{
	CS_DEBUG()
		<< "Dispose request got from socket " << socketUuid
		<< " (index " << index << ")"
		<< std::endl;

	std::vector<Socket *>::iterator removeTargetIterator = _SocketList.begin() + index;
	delete *removeTargetIterator;
	_SocketList.erase(
		removeTargetIterator
	);

	CS_LOG()
		<< "Socket " << socketUuid << " removed"
		<< std::endl;
}

}	 // namespace webserv