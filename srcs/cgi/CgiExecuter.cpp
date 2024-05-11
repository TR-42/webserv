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
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger,
	std::vector<Pollable *> &pollableList
) : Pollable(-1),
		_fdWriteToCgi(-1),
		_pid(0),
		_cgiHandler(NULL),
		logger(logger)
{
	// argvを準備
	char **argv = new char *[2];
	argv[0] = new char[request.getPath().size() + 1];
	std::strcpy(argv[0], request.getPath().c_str());
	argv[1] = NULL;

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

	int fdReadFromCgi = pipeFd[PIPE_READ];
	int fdWriteToParent = pipeFd[PIPE_WRITE];

	this->_cgiHandler = new CgiHandler(
		errorPageProvider,
		logger,
		fdReadFromCgi
	);
	pollableList.push_back(this->_cgiHandler);
	// 自分自身までpollableListに追加するとdisposeで面倒なことになるので、親に管理してもらう。

	// fork
	this->_pid = fork();
	if (this->_pid < 0) {
		errno_t err = errno;
		CS_ERROR() << "fork() failed: " << std::strerror(err) << std::endl;
		close(this->_fdWriteToCgi);
		this->_fdWriteToCgi = -1;
		close(fdReadFromParent);
		close(fdReadFromCgi);
		close(fdWriteToParent);
		throw std::runtime_error("fork() failed");
	}

	if (this->_pid == 0) {
		close(this->_fdWriteToCgi);
		this->_fdWriteToCgi = -1;
		close(fdReadFromCgi);
		// child process
		this->_childProcessFunc(
			pollableList,
			fdReadFromParent,
			fdWriteToParent,
			request.getPath(),
			argv,
			envp
		);
	} else {
		// parent process
		env::EnvManager::freeEnvp(&argv);
		envManager.freeEnvp(&envp);
		close(fdReadFromParent);
		close(fdWriteToParent);
	}
}

__attribute__((noreturn)) void CgiExecuter::_childProcessFunc(
	std::vector<Pollable *> &pollableList,
	int fdReadFromParent,
	int fdWriteToParent,
	const std::string &cgiPath,
	char **argv,
	char **envp
)
{
	size_t pollableListSize = pollableList.size();
	for (size_t i = 0; i < pollableListSize; i++) {
		// TODO: 自分自身をdeleteしないようにする
		if (pollableList[i] == NULL) {
			continue;
		}
		delete pollableList[i];
		pollableList[i] = NULL;
	}
	pollableList.clear();

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

	int result = execve(cgiPath.c_str(), argv, envp);
	errno_t err = errno;
	CS_ERROR() << "execve() failed with code " << result << ": " << std::strerror(err) << std::endl;
	std::exit(1);
}

CgiExecuter::~CgiExecuter()
{
	if (0 <= this->_fdWriteToCgi) {
		close(this->_fdWriteToCgi);
	}

	if (0 < this->_pid) {
		int status;
		CS_DEBUG() << "waiting for child process to terminate ... pid=" << this->_pid << std::endl;
		waitpid(this->_pid, &status, 0);
		CS_DEBUG() << "waitpid() status=" << status << std::endl;
	} else {
		CS_ERROR() << "fork() may have been failed" << std::endl;
	}
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
	short revents,
	std::vector<Pollable *> &pollableList
)
{
	(void)pollableList;
	if (!IS_POLLOUT(revents)) {
		return PollEventResult::OK;
	}

	// TODO: implement

	// readerのcompleteによって、自動的にwriterもcompleteになる
	return PollEventResult::OK;
}

CgiHandler *CgiExecuter::getCgiHandler() const
{
	return this->_cgiHandler;
}

}	 // namespace webserv
