#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cgi/CgiExecuter.hpp>
#include <climits>
#include <cstdlib>
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
	const std::string &workingDir,
	const Logger &logger,
	int fdWriteToCgi,
	int fdReadFromParent,
	int fdReadFromCgi,
	int fdWriteToParent
) : _requestBody(requestBody),
		_fdWriteToCgi(fdWriteToCgi),
		_pid(-1),
		logger(logger),
		_writtenCount(0)
{
	CS_DEBUG()
		<< "initializing... "
		<< "workingDir=" << workingDir
		<< ", requestBodyLength=" << this->_requestBody.size()
		<< std::endl;

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
			workingDir,
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

CgiExecuter::CgiExecuter(
	const CgiExecuter &src
) : _requestBody(src._requestBody)
{
	(void)src;
	throw std::runtime_error("CgiExecuter copy constructor is not allowed");
}

CgiExecuter &CgiExecuter::operator=(
	const CgiExecuter &src
)
{
	(void)src;
	throw std::runtime_error("CgiExecuter copy assignment operator is not allowed");
}

__attribute__((noreturn)) void CgiExecuter::_childProcessFunc(
	std::string workingDir,
	int fdReadFromParent,
	int fdWriteToParent,
	char **argv,
	char **envp
)
{
	C_DEBUG("child process started");

	CS_DEBUG()
		<< "Working directory changing to " << workingDir
		<< std::endl;
	if (chdir(workingDir.c_str()) < 0) {
		errno_t err = errno;
		CS_ERROR()
			<< "chdir() failed: " << std::strerror(err)
			<< std::endl;
		std::exit(1);
	}

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

	size_t sizeToWrite = this->_requestBody.size() - this->_writtenCount;
	if (INT_MAX < sizeToWrite) {
		sizeToWrite = INT_MAX;
	}
	ssize_t writtenCount = write(
		this->_fdWriteToCgi,
		this->_requestBody.data() + this->_writtenCount,
		sizeToWrite
	);

	if (writtenCount < 0) {
		errno_t err = errno;
		CS_ERROR() << "write() failed: " << std::strerror(err) << std::endl;
		return PollEventResult::ERROR;
	}

	this->_writtenCount += writtenCount;
	CS_DEBUG()
		<< "written "
		<< this->_writtenCount
		<< " / "
		<< this->_requestBody.size()
		<< " bytes to CGI"
		<< ", current writtenCount=" << writtenCount
		<< std::endl;

	if (this->isWriteToCgiCompleted()) {
		C_DEBUG("write to CGI completed");
		return PollEventResult::DISPOSE_REQUEST;
	}

	return PollEventResult::OK;
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
