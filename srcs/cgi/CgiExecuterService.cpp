#include <stdlib.h>
#include <unistd.h>

#include <EnvManager.hpp>
#include <cgi/CgiExecuterService.hpp>
#include <iostream>
#include <macros.hpp>
#include <stdexcept>

namespace webserv
{

#define PIPE_READ 0
#define PIPE_WRITE 1

CgiExecuterService::CgiExecuterService(
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger,
	std::vector<Socket *> &sockets
) : ServiceBase(request, errorPageProvider, logger),
		_fdWriteToCgi(-1),
		_fdReadFromCgi(-1),
		_isReaderInstanceCreated(false),
		_pid(0)
{
	// 環境変数を準備
	env::EnvManager envManager;

	char **envp = envManager.toEnvp();
	if (envp == NULL) {
		CS_ERROR() << "envManager.toEnvp() failed" << std::endl;
		throw std::runtime_error("envManager.toEnvp() failed");
	}

	// pipeを作成
	int pipeFd[2];
	if (pipe(pipeFd) < 0) {
		errno_t err = errno;
		CS_ERROR() << "pipe() failed: " << std::strerror(err) << std::endl;
		throw std::runtime_error("pipe() failed");
	}

	this->_fdWriteToCgi = pipeFd[PIPE_WRITE];
	int fdReadFromParent = pipeFd[PIPE_READ];

	if (pipe(pipeFd) < 0) {
		errno_t err = errno;
		CS_ERROR() << "pipe() failed" << std::strerror(err) << std::endl;
		close(this->_fdWriteToCgi);
		this->_fdWriteToCgi = -1;
		close(fdReadFromParent);
		throw std::runtime_error("pipe() failed");
	}

	this->_fdReadFromCgi = pipeFd[PIPE_READ];
	int fdWriteToParent = pipeFd[PIPE_WRITE];

	// fork
	this->_pid = fork();
	if (this->_pid < 0) {
		errno_t err = errno;
		CS_ERROR() << "fork() failed: " << std::strerror(err) << std::endl;
		close(this->_fdWriteToCgi);
		this->_fdWriteToCgi = -1;
		close(fdReadFromParent);
		close(this->_fdReadFromCgi);
		this->_fdReadFromCgi = -1;
		close(fdWriteToParent);
		throw std::runtime_error("fork() failed");
	}

	if (this->_pid == 0) {
		// child process
		this->_childProcessFunc(
			sockets,
			fdReadFromParent,
			fdWriteToParent,
			request.getPath(),
			envp
		);
	} else {
		// parent process
		envManager.freeEnvp(&envp);
		close(fdReadFromParent);
		close(fdWriteToParent);
	}
}

__attribute__((noreturn)) void CgiExecuterService::_childProcessFunc(
	std::vector<Socket *> &sockets,
	int fdReadFromParent,
	int fdWriteToParent,
	const std::string &cgiPath,
	char **envp
)
{
	close(this->_fdReadFromCgi);
	close(this->_fdWriteToCgi);

	size_t socketListSize = sockets.size();
	for (size_t i = 0; i < socketListSize; i++) {
		// TODO: 自分自身をdeleteしないようにする
		if (sockets[i] == NULL) {
			continue;
		}
		delete sockets[i];
		sockets[i] = NULL;
	}
	sockets.clear();

	// stdin, stdoutをpipeに接続
	if (dup2(fdReadFromParent, STDIN_FILENO) < 0) {
		errno_t err = errno;
		CS_ERROR() << "dup2() failed: " << std::strerror(err) << std::endl;
		std::exit(1);
	}
	close(fdReadFromParent);

	if (dup2(fdWriteToParent, STDOUT_FILENO) < 0) {
		errno_t err = errno;
		CS_ERROR() << "dup2() failed: " << std::strerror(err) << std::endl;
		std::exit(1);
	}
	close(fdWriteToParent);

	int result = execve(cgiPath.c_str(), NULL, envp);
	errno_t err = errno;
	CS_ERROR() << "execve() failed with code " << result << ": " << std::strerror(err) << std::endl;
	std::exit(1);
}

CgiExecuterService::~CgiExecuterService()
{
	if (0 <= this->_fdWriteToCgi) {
		close(this->_fdWriteToCgi);
	}
	if (0 <= this->_fdReadFromCgi) {
		close(this->_fdReadFromCgi);
	}

	if (0 < this->_pid) {
		int status;
		CS_DEBUG() << "waiting for child process to terminate ... pid=" << this->_pid << std::endl;
		waitpid(this->_pid, &status, 0);
		CS_DEBUG() << "waitpid() status=" << status << std::endl;

	} else {
		CS_ERROR() << "fork() failed" << std::endl;
	}
}

void CgiExecuterService::setToPollFd(
	pollfd &pollFd
) const
{
	pollFd.fd = this->_fdWriteToCgi;
	pollFd.events = POLLOUT;
	pollFd.revents = 0;
}

ServiceEventResultType CgiExecuterService::onEventGot(
	short revents
)
{
	if (!IS_POLLOUT(revents)) {
		return ServiceEventResult::CONTINUE;
	}

	// TODO: implement

	// readerのcompleteによって、自動的にwriterもcompleteになる
	return ServiceEventResult::CONTINUE;
}

bool CgiExecuterService::isWriterInstance() const
{
	return true;
}

CgiHandlerService *CgiExecuterService::createCgiHandlerService()
{
	if (this->_isReaderInstanceCreated) {
		CS_ERROR() << "CgiHandlerService instance already created" << std::endl;
		throw std::runtime_error("CgiHandlerService instance already created");
		return NULL;
	}

	this->_isReaderInstanceCreated = true;
	return new CgiHandlerService(
		this->_request,
		this->_errorPageProvider,
		this->logger,
		this->_fdReadFromCgi
	);
}

}	 // namespace webserv
