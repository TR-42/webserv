#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <EnvManager.hpp>
#include <cerrno>
#include <macros.hpp>
#include <service/CgiService.hpp>
#include <utils/waitResultStatusToString.hpp>

#define STR(v) #v

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
	const RequestedFileInfo &requestedFileInfo,
	const utils::ErrorPageProvider &errorPageProvider,
	const Logger &logger,
	std::vector<Pollable *> &pollableList
) : ServiceBase(request, errorPageProvider, logger),
		_pid(-1),
		_cgiExecuter(NULL),
		_cgiHandler(NULL)
{
	C_DEBUG("initializing...");

	const CgiConfig &cgiConfig = requestedFileInfo.getCgiConfig();
	const std::string &cgiPath = cgiConfig.getCgiExecutableFullPath();

	// argvを準備
	char **argv = new char *[3];
	argv[0] = new char[cgiPath.size() + 1];
	std::strcpy(argv[0], cgiPath.c_str());
	std::string pathTranslated = requestedFileInfo.getTargetFilePath();
	argv[1] = new char[pathTranslated.size() + 1];
	std::strcpy(argv[1], pathTranslated.c_str());
	argv[2] = NULL;
	CS_DEBUG()
		<< "argv[0]: " << argv[0]
		<< ", argv[1]: " << argv[1]
		<< std::endl;

	// 環境変数を準備
	env::EnvManager envManager(cgiConfig.getEnvPreset());
	envManager.set("GATEWAY_INTERFACE", "CGI/1.1");
	// /abc/index.php/extra/def の場合、PATH_INFOは /extra/def
	envManager.set("PATH_INFO", requestedFileInfo.getCgiPathInfo());
	envManager.set("PATH_TRANSLATED", pathTranslated);
	envManager.set("QUERY_STRING", request.getQuery());
	envManager.set("REQUEST_METHOD", request.getMethod());
	// /abc/index.php/extra/def の場合、SCRIPT_NAMEは /abc/index.php
	// /abc の場合、SCRIPT_NAMEは /abc/index.php
	envManager.set("SCRIPT_NAME", requestedFileInfo.getCgiScriptName());
	// TODO: アドレス系実装
	envManager.set("REMOTE_ADDR", "127.0.0.1");
	envManager.set("REMOTE_HOST", "localhost");
	envManager.set("REMOTE_PORT", "12345");
	envManager.set("SERVER_NAME", "localhost");
	envManager.set("SERVER_PORT", "80");

	envManager.set("SERVER_PROTOCOL", request.getVersion());
	envManager.set("SERVER_SOFTWARE", "webserv/1.0");

	char **envp = envManager.toEnvp();
	if (envp == NULL) {
		CS_ERROR() << "envManager.toEnvp() failed" << std::endl;
		env::EnvManager::freeEnvp(&argv);
		return;
	}

	// pipeを作成
	C_DEBUG("Creating pipe...");
	int fdWriteToCgi = -1;
	int fdReadFromParent = -1;
	int fdReadFromCgi = -1;
	int fdWriteToParent = -1;
	if (!_preparePipe(fdWriteToCgi, fdReadFromParent, fdReadFromCgi, fdWriteToParent, logger)) {
		CS_ERROR() << "_preparePipe() failed" << std::endl;
		env::EnvManager::freeEnvp(&argv);
		env::EnvManager::freeEnvp(&envp);
		return;
	}

	CS_DEBUG()
		<< "fdWriteToCgi: " << fdWriteToCgi
		<< ", fdReadFromParent: " << fdReadFromParent
		<< ", fdReadFromCgi: " << fdReadFromCgi
		<< ", fdWriteToParent: " << fdWriteToParent
		<< std::endl;

	this->_cgiExecuter = new CgiExecuter(
		request.getBody(),
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

	CS_DEBUG()
		<< "CgiExecuter created: "
		<< "pid: " << this->_cgiExecuter->getPid()
		<< std::endl;

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

	this->_cgiHandler = new CgiHandler(
		errorPageProvider,
		logger,
		fdReadFromCgi,
		&(this->_cgiHandler),
		&(this->_response)
	);

	CS_DEBUG()
		<< "CgiHandler created"
		<< std::endl;

	pollableList.push_back(this->_cgiHandler);

	// レスポンスが返ってこなかった場合等はInternalServerErrorなので、あらかじめここで設定しておく
	this->_response = errorPageProvider.internalServerError();

	C_DEBUG("initialized");
}

CgiService::~CgiService()
{
	if (this->_cgiExecuter != NULL) {
		delete this->_cgiExecuter;
		this->_cgiExecuter = NULL;
	}

	if (this->_cgiHandler != NULL) {
		this->_cgiHandler->setDisposeRequested();
		this->_cgiHandler = NULL;
	}

	// 他の子プロセスから閉じられる可能性があるため。
	if (!this->isDisposingFromChildProcess() && 0 < this->_pid) {
		// 子プロセスが終了するまで待つ
		int status;
		int waitResult = waitpid(this->_pid, &status, WNOHANG);
		if (waitResult < 0) {
			errno_t err = errno;
			CS_ERROR() << "waitpid() failed: " << std::strerror(err) << std::endl;
		} else if (waitResult == 0) {
			CS_DEBUG() << "waitpid() returned 0" << std::endl;
		} else {
			CS_INFO() << "waitpid() returned " << waitResult << " with " << utils::waitResultStatusToString(status) << std::endl;
		}

		if (waitResult == 0) {
			// 子プロセスが終了していない場合、強制終了
			if (kill(this->_pid, SIGKILL) < 0) {
				errno_t err = errno;
				CS_ERROR() << "kill() failed: " << std::strerror(err) << std::endl;
			}
			CS_INFO() << "kill() called with signal" STR(SIGKILL) << std::endl;

			waitResult = waitpid(this->_pid, &status, 0);
			if (waitResult < 0) {
				errno_t err = errno;
				CS_ERROR() << "waitpid() retry failed: " << std::strerror(err) << std::endl;
			} else if (waitResult == 0) {
				CS_DEBUG() << "waitpid() retry returned 0" << std::endl;
			} else {
				CS_INFO() << "waitpid() retry returned " << waitResult << " with " << utils::waitResultStatusToString(status) << std::endl;
			}
		}

		this->_pid = -1;
	}
}

void CgiService::setToPollFd(
	struct pollfd &pollFd
) const
{
	if (this->_cgiExecuter == NULL) {
		// CgiExecuterが終了している場合はCgiExecuterのFDをセットする必要もないため、親のFDを確認してもらう
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
	if (IS_POLL_ANY_ERROR(revents)) {
		// 子プロセスの終了によるエラーの場合は、サービス自体を終了する
		C_DEBUG("IS_POLL_ANY_ERROR(revents)");
		if (this->_pid <= 0) {
			C_ERROR("this->_pid <= 0");
			return ServiceEventResult::ERROR;
		}

		int status;
		int waitResult = waitpid(this->_pid, &status, WNOHANG);
		if (waitResult < 0) {
			errno_t err = errno;
			CS_ERROR() << "waitpid() failed: " << std::strerror(err) << std::endl;
			return ServiceEventResult::ERROR;
		} else if (waitResult == 0) {
			CS_DEBUG() << "waitpid() returned 0 -> handle it in CgiExecuter" << std::endl;
		} else {
			CS_INFO() << "waitpid() returned " << waitResult << " with " << utils::waitResultStatusToString(status) << std::endl;
			this->_pid = -1;
			return ServiceEventResult::ERROR;
		}
	}

	if (this->_cgiHandler == NULL) {
		if (this->_pid <= 0) {
			C_DEBUG("this->_pid <= 0");
			return ServiceEventResult::COMPLETE;
		}

		if (this->_cgiExecuter != NULL) {
			C_DEBUG("this->_cgiExecuter != NULL -> dispose");
			delete this->_cgiExecuter;
			this->_cgiExecuter = NULL;
			if (kill(this->_pid, SIGKILL) < 0) {
				errno_t err = errno;
				CS_ERROR() << "kill() failed: " << std::strerror(err) << std::endl;
				return ServiceEventResult::ERROR;
			}
		}

		// waitpidをイベントループ内で行なってしまう
		int status;
		int waitResult = waitpid(this->_pid, &status, WNOHANG);
		if (waitResult < 0) {
			errno_t err = errno;
			CS_ERROR() << "waitpid() failed: " << std::strerror(err) << std::endl;
			// エラーの場合は強制終了する
			if (kill(this->_pid, SIGKILL) < 0) {
				err = errno;
				CS_ERROR() << "kill() failed: " << std::strerror(err) << std::endl;
			}
			CS_INFO() << "kill() called with signal" STR(SIGKILL) << std::endl;
			return ServiceEventResult::ERROR;
		} else if (waitResult == 0) {
			CS_DEBUG() << "waitpid() returned 0" << std::endl;
			return ServiceEventResult::CONTINUE;
		} else {
			CS_INFO() << "waitpid() returned " << waitResult << " with " << utils::waitResultStatusToString(status) << std::endl;
			this->_pid = -1;
			return ServiceEventResult::COMPLETE;
		}
	}

	if (this->_cgiExecuter == NULL) {
		C_DEBUG("this->_cgiExecuter == NULL ... continue");
		return ServiceEventResult::CONTINUE;
	}

	PollEventResultType executerResult = this->_cgiExecuter->onEventGot(revents);
	switch (executerResult) {
		case PollEventResult::OK:
			C_DEBUG("Executer OK");
			return ServiceEventResult::CONTINUE;

		case PollEventResult::ERROR:
			C_ERROR("Executer ERROR");
			this->_cgiHandler->setDisposeRequested();
			this->_cgiHandler = NULL;
			return ServiceEventResult::ERROR;

		case PollEventResult::DISPOSE_REQUEST:
			C_DEBUG("Executer DISPOSE_REQUEST");
			delete this->_cgiExecuter;
			this->_cgiExecuter = NULL;
			// Handler側が終了するまで、このService自体は続行する
			return ServiceEventResult::CONTINUE;
	}

	// ここに来るはずはない
	C_ERROR("Unexpected result from CgiExecuter");
	this->_cgiHandler->setDisposeRequested();
	this->_cgiHandler = NULL;
	return ServiceEventResult::ERROR;
}

}	 // namespace webserv
