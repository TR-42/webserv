#include <iostream>
#include <macros.hpp>
#include <poll/Poll.hpp>
#include <stdexcept>
#include <utils/to_string.hpp>

#define POLL_TIMEOUT 1000

namespace webserv
{

Poll::Poll(
	const std::vector<Pollable *> &initialPollableList,
	const Logger &logger
) : _PollableList(initialPollableList),
		logger(logger)
{
}

Poll::~Poll()
{
	const size_t pollableCount = _PollableList.size();
	CS_DEBUG()
		<< "Poll instance destroying... "
		<< pollableCount << " pollable instances left"
		<< std::endl;

	for (size_t i = 0; i < pollableCount; i++) {
		delete this->_PollableList[i];
		// 念のためNULLを代入
		this->_PollableList[i] = NULL;
	}

	// 念のためクリア
	this->_PollableList.clear();
}

bool Poll::loop()
{
	std::vector<Pollable *> _PollableListCopy(_PollableList);
	const size_t pollableCount = _PollableListCopy.size();
	_PollFdList.resize(pollableCount);
	for (size_t i = 0; i < pollableCount; i++) {
		_PollableListCopy[i]->setToPollFd(_PollFdList[i]);
	}

	const int pollResult = poll(
		_PollFdList.data(),
		pollableCount,
		POLL_TIMEOUT
	);
	if (pollResult < 0) {
		const char *errorStr = strerror(errno);
		CS_FATAL()
			<< "poll() failed: " << errorStr
			<< std::endl;
		return false;
	}

	size_t removedCount = 0;
	for (size_t i = 0; i < pollableCount; i++) {
		utils::UUID pollableUuid = _PollableListCopy[i]->getUUID();

		int fd = _PollFdList[i].fd;
		short revents = _PollFdList[i].revents;
		bool isFdSame = _PollableListCopy[i]->isFdSame(fd);
		bool isPollErr = IS_POLLERR(revents) != 0;
		bool isPollHup = IS_POLLHUP(revents) != 0;
		bool isPollNval = IS_POLLNVAL(revents) != 0;
		if (isFdSame && (isPollErr || isPollHup || isPollNval)) {
			CS_WARN()
				<< "Error event got from pollable " << pollableUuid
				<< " ("
				<< "fd: " << fd
				<< ", pollerr: " << std::boolalpha << isPollErr
				<< ", pollhup: " << std::boolalpha << isPollHup
				<< ", pollnval: " << std::boolalpha << isPollNval
				<< ", revents: " << revents
				<< ")"
				<< std::endl;
			this->_onPollableDisposeRequested(i - removedCount, pollableUuid);
			++removedCount;
			continue;
		}

		const PollEventResultType result = _PollableListCopy[i]->onEventGot(
			fd,
			revents,
			_PollableList
		);

		switch (result) {
			case PollEventResult::OK:
				// OKなら何もしない
				break;

			case PollEventResult::DISPOSE_REQUEST:
				this->_onPollableDisposeRequested(i - removedCount, pollableUuid);
				++removedCount;
				break;

			case PollEventResult::ERROR:
				CS_FATAL()
					<< "Error got from pollable " << pollableUuid
					<< std::endl;
				return false;

			default:
				CS_FATAL()
					<< "Unknown SockEventResultType: " << utils::to_string(result)
					<< " from pollable " << pollableUuid
					<< std::endl;
				return false;
		}
	}

	return true;
}

void Poll::_onPollableDisposeRequested(
	size_t index,
	utils::UUID pollableUuid
)
{
	CS_DEBUG()
		<< "Dispose request got from pollable " << pollableUuid
		<< " (index " << index << ")"
		<< std::endl;

	std::vector<Pollable *>::iterator removeTargetIterator = _PollableList.begin() + index;
	delete *removeTargetIterator;
	_PollableList.erase(
		removeTargetIterator
	);

	CS_LOG()
		<< "Pollable " << pollableUuid << " removed"
		<< std::endl;
}

}	 // namespace webserv
