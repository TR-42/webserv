#include <sys/wait.h>
#include <unistd.h>

#include <EnvManager.hpp>
#include <cgi/CgiExecuter.hpp>
#include <cstring>
#include <iostream>
#include <macros.hpp>
#include <stdexcept>

namespace webserv
{

#define PIPE_READ 0
#define PIPE_WRITE 1

CgiExecuter::CgiExecuter(
	const std::vector<uint8_t> &requestBody,
	char **argv,
	char **envp,
	const Logger &logger,
	int fdWriteToCgi,
	int fdReadFromParent,
	int fdReadFromCgi,
	int fdWriteToParent,
	std::vector<Pollable *> &pollableList
) : _requestBody(requestBody),
		_fdWriteToCgi(fdWriteToCgi),
		_pid(-1),
		logger(logger),
		_writtenCount(0)
{
	C_DEBUG("initializing...");
	this->_pid = fork();
	if (this->_pid < 0) {
		errno_t err = errno;
		CS_ERROR() << "fork() failed: " << std::strerror(err) << std::endl;
		return;
	}

	if (this->_pid == 0) {
		close(this->_fdWriteToCgi);
		this->_fdWriteToCgi = -1;
		close(fdReadFromCgi);
		// child process
		// noreturn
		this->_childProcessFunc(
			pollableList,
			fdReadFromParent,
			fdWriteToParent,
			argv,
			envp
		);
	}

	CS_DEBUG()
		<< "forked: pid=" << this->_pid
		<< std::endl;

	// 親から渡されたリソースの解放は親が行う (fdWriteToCgiだけはdestructorで自分で閉じる)
}

__attribute__((noreturn)) void CgiExecuter::_childProcessFunc(
	std::vector<Pollable *> &pollableList,
	int fdReadFromParent,
	int fdWriteToParent,
	char **argv,
	char **envp
)
{
	C_DEBUG("child process started");
	size_t pollableListSize = pollableList.size();
	for (size_t i = 0; i < pollableListSize; i++) {
		if (pollableList[i] == NULL) {
			continue;
		}
		pollableList[i]->setIsDisposingFromChildProcess(true);
		delete pollableList[i];
		pollableList[i] = NULL;
	}
	pollableList.clear();

	// stdin, stdoutをpipeに接続
	C_DEBUG("Connecting stdin to pipe...");
	if (dup2(fdReadFromParent, STDIN_FILENO) < 0) {
		errno_t err = errno;
		CS_ERROR() << "dup2() failed: " << std::strerror(err) << std::endl;
		close(fdReadFromParent);
		close(fdWriteToParent);
		std::exit(1);
	}
	close(fdReadFromParent);

	C_DEBUG("Connecting stdout to pipe...");
	if (dup2(fdWriteToParent, STDOUT_FILENO) < 0) {
		errno_t err = errno;
		CS_ERROR() << "dup2() failed: " << std::strerror(err) << std::endl;
		close(fdWriteToParent);
		std::exit(1);
	}
	close(fdWriteToParent);

	int result = execve(argv[0], argv, envp);
	errno_t err = errno;
	CS_ERROR() << "execve() failed with code " << result << ": " << std::strerror(err) << std::endl;
	std::exit(1);
}

CgiExecuter::~CgiExecuter()
{
	if (0 <= this->_fdWriteToCgi) {
		close(this->_fdWriteToCgi);
		CS_DEBUG()
			<< "closed fdWriteToCgi: " << this->_fdWriteToCgi
			<< std::endl;
	}
	C_DEBUG("disposed");
}

void CgiExecuter::setToPollFd(
	pollfd &pollFd
) const
{
	pollFd.fd = this->_fdWriteToCgi;
	pollFd.events = POLLOUT;
	pollFd.revents = 0;
}

PollEventResultType CgiExecuter::onEventGot(
	short revents
)
{
	if (!IS_POLLOUT(revents)) {
		return PollEventResult::OK;
	}

	ssize_t writtenCount = write(
		this->_fdWriteToCgi,
		this->_requestBody.data() + this->_writtenCount,
		this->_requestBody.size() - this->_writtenCount
	);

	if (writtenCount < 0) {
		errno_t err = errno;
		CS_ERROR() << "write() failed: " << std::strerror(err) << std::endl;
		return PollEventResult::ERROR;
	}

	if (this->isWriteToCgiCompleted()) {
		return PollEventResult::DISPOSE_REQUEST;
	}

	return PollEventResult::OK;
}
PollEventResultType CgiExecuter::onEventGot(
	short revents,
	std::vector<Pollable *> &pollableList
)
{
	(void)pollableList;
	return this->onEventGot(revents);
}

pid_t CgiExecuter::getPid() const
{
	return this->_pid;
}

bool CgiExecuter::isWriteToCgiCompleted() const
{
	return this->_requestBody.size() <= this->_writtenCount;
}

}	 // namespace webserv
