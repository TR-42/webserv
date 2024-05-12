#include <unistd.h>

#include <EnvManager.hpp>
#include <service/CgiService.hpp>

namespace webserv
{

#define PIPE_READ 0
#define PIPE_WRITE 1

static bool _preparePipe(
	int &fdWriteToCgi,
	int &fdReadFromParent,
	int &fdReadFromCgi,
	int &fdWriteToParent,
	const Logger &logger
)
{
	int pipeFd[2];
	if (pipe(pipeFd) < 0) {
		errno_t err = errno;
		LS_ERROR() << "pipe() failed: " << std::strerror(err) << std::endl;
		return false;
	}

	fdWriteToCgi = pipeFd[PIPE_WRITE];
	fdReadFromParent = pipeFd[PIPE_READ];

	if (pipe(pipeFd) < 0) {
		errno_t err = errno;
		LS_ERROR() << "pipe() failed" << std::strerror(err) << std::endl;
		close(fdWriteToCgi);
		close(fdReadFromParent);
		return false;
	}

	fdReadFromCgi = pipeFd[PIPE_READ];
	fdWriteToParent = pipeFd[PIPE_WRITE];

	return true;
}

CgiService::CgiService(
	const HttpRequest &request,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger,
	std::vector<Pollable *> &pollableList
) : ServiceBase(request, errorPageProvider, logger),
		_pid(-1),
		_cgiExecuter(NULL),
		_cgiHandler(NULL)
{
	(void)this->_cgiHandler;

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
		env::EnvManager::freeEnvp(&argv);
		return;
	}

	// pipeを作成
	int fdWriteToCgi = -1;
	int fdReadFromParent = -1;
	int fdReadFromCgi = -1;
	int fdWriteToParent = -1;
	if (!_preparePipe(fdWriteToCgi, fdReadFromParent, fdReadFromCgi, fdWriteToParent, logger)) {
		env::EnvManager::freeEnvp(&argv);
		env::EnvManager::freeEnvp(&envp);
		return;
	}

	this->_cgiExecuter = new CgiExecuter(
		request.getBody(),
		request.getPath(),
		argv,
		envp,
		logger,
		fdWriteToCgi,
		fdReadFromParent,
		fdReadFromCgi,
		fdWriteToParent,
		pollableList
	);
	env::EnvManager::freeEnvp(&argv);
	env::EnvManager::freeEnvp(&envp);
	// fdWriteToCgiはCgiExecuterで閉じる
	close(fdReadFromParent);
	close(fdWriteToParent);
	// fdReadFromCgiはCgiHandlerで使う & そちらで閉じる

	this->_pid = this->_cgiExecuter->getPid();
	if (this->_pid < 0) {
		CS_ERROR() << "this->_pid < 0" << std::endl;
		delete this->_cgiExecuter;
		this->_cgiExecuter = NULL;
		return;
	}

	if (this->_cgiExecuter->isWriteToCgiCompleted()) {
		CS_INFO() << "this->_cgiExecuter->isWriteToCgiCompleted() == true" << std::endl;
		delete this->_cgiExecuter;
		this->_cgiExecuter = NULL;
	}

	// TODO: CgiHandlerを作成

	if (this->_cgiHandler != NULL) {
		pollableList.push_back(this->_cgiHandler);
	}
}

CgiService::~CgiService()
{
	if (this->_cgiExecuter != NULL) {
		delete this->_cgiExecuter;
		this->_cgiExecuter = NULL;
	}
}

void CgiService::setToPollFd(
	struct pollfd &pollFd
) const
{
	if (this->_cgiExecuter == NULL) {
		pollFd.fd = 0;
		pollFd.events = 0;
		pollFd.revents = 0;
	} else {
		this->_cgiExecuter->setToPollFd(pollFd);
	}
}

ServiceEventResultType CgiService::onEventGot(
	short revents
)
{
	(void)revents;
	if (this->_cgiHandler == NULL) {
		CS_ERROR() << "this->_cgiExecuter == NULL || this->_cgiHandler == NULL" << std::endl;
		return ServiceEventResult::ERROR;
	}

	if (this->_cgiExecuter == NULL) {
		// TODO:　Handlerの終了を検知する -> 条件を満たした場合のみCOMPLETEにする
		return ServiceEventResult::COMPLETE;
	}

	PollEventResultType executerResult = this->_cgiExecuter->onEventGot(revents);
	switch (executerResult) {
		case PollEventResult::OK:
			return ServiceEventResult::CONTINUE;
		case PollEventResult::ERROR:
			// TODO: エラーの場合はHandler側にエラー通知する
			return ServiceEventResult::ERROR;
		case PollEventResult::DISPOSE_REQUEST:
			delete this->_cgiExecuter;
			this->_cgiExecuter = NULL;
			// Handler側が終了するまで、このService自体は続行する
			return ServiceEventResult::CONTINUE;
	}

	// ここに来るはずはない
	return ServiceEventResult::ERROR;
}

}	 // namespace webserv
