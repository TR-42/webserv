#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

#include <Logger.hpp>
#include <cerrno>
#include <config/ServerRunningConfig.hpp>
#include <config/validateConfig.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <poll/Poll.hpp>
#include <signal/signal_handler.hpp>
#include <socket/ServerSocket.hpp>
#include <string>
#include <utils/getTimeStr.hpp>
#include <utils/to_string.hpp>

#include "config/parseListenConfig.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "utils/ErrorPageProvider.hpp"
#include "yaml/YamlParser.hpp"

#define DEFAULT_CONFIG_FILE_PATH "webserv.yaml"

typedef std::vector<webserv::Pollable *> PollableList;

static void __size_check();

static std::string get_argv_str(int argc, const char *argv[])
{
	std::string str;
	for (int i = 0; i < argc; i++) {
		if (i != 0) {
			str += " ";
		}
		str += "[" + webserv::utils::to_string(i) + "]:'" + argv[i] + "'";
	}
	return str;
}

static void generatePidFile(
	const char *argv0,
	const webserv::Logger &logger
)
{
#ifdef DEBUG
	std::string pidFilePath(std::string(argv0) + ".pid");
	pid_t pid = getpid();
	std::ofstream pidFile;

	pidFile.open(pidFilePath.c_str(), std::ios_base::out | std::ios_base::trunc);
	pidFile << pid;
	pidFile.close();

	LS_INFO()
		<< "pid: " << std::dec << pid
		<< " pidFilePath: " << pidFilePath
		<< std::endl;
#else
	(void)argv0;
	(void)logger;
#endif
}

static bool loadConfigFile(
	const std::string &configFilePath,
	webserv::Logger &logger,
	webserv::ListenConfig &listenConfig
)
{
	LS_INFO()
		<< "configFilePath: " << configFilePath
		<< std::endl;

	webserv::yaml::MappingNode root("");

	std::ifstream ifs(configFilePath.c_str());
	if (!ifs) {
		LS_FATAL()
			<< "failed to open file: " << configFilePath
			<< std::endl;
		return false;
	}

	try {
		if (!webserv::yaml::parse(
					ifs,
					root,
					logger
				)) {
			L_FATAL("yaml::parse failed");
			return false;
		}
		listenConfig = webserv::parseListenConfig(root, configFilePath);
	} catch (const std::exception &e) {
		LS_FATAL()
			<< "parseListenConfig failed: " << e.what()
			<< std::endl;
		return false;
	}

	std::vector<std::string> errorMessageList = webserv::validateConfig(listenConfig);
	if (!errorMessageList.empty()) {
		LS_FATAL()
			<< "validateConfig failed: errorMessageList.size(): " << errorMessageList.size()
			<< std::endl;
		for (size_t i = 0; i < errorMessageList.size(); ++i) {
			LS_ERROR()
				<< "[" << std::dec << i << "]: " << errorMessageList[i]
				<< std::endl;
		}
		return false;
	}

	return true;
}

int main(int argc, const char *argv[])
{
	__size_check();

#ifdef DEBUG
	std::ofstream logFile;
	std::string logFilePath("./logs/webserv." + webserv::utils::getIso8601ShortTimeStr() + ".log");
	try {
		logFile.open(logFilePath.c_str(), std::ios_base::app);
	} catch (const std::exception &e) {
		std::cerr << "logFile.open failed: " << e.what() << std::endl;
		return 1;
	}

	webserv::Logger logger(logFile);
#else
	webserv::Logger logger;
#endif
	webserv::utils::ErrorPageProvider errorPageProvider;

	generatePidFile(argv[0], logger);

	L_LOG("argv: " + get_argv_str(argc, argv));

	webserv::ListenConfig listenConfig;

	try {
		if (!loadConfigFile(1 < argc ? argv[1] : DEFAULT_CONFIG_FILE_PATH, logger, listenConfig)) {
			L_FATAL("loadConfigFile failed");
			return 1;
		}

		if (!webserv::registerSignalHandler()) {
			L_FATAL("registerSignalHandler failed");
			return 1;
		}
	} catch (const std::exception &e) {
		LS_FATAL()
			<< "loadConfigFile/registerSignalHandler failed: " << e.what()
			<< std::endl;
		return 1;
	}

	PollableList pollableList;
	try {
		for (webserv::ListenMapType::const_iterator it = listenConfig.getListenMap().begin();
				 it != listenConfig.getListenMap().end();
				 ++it) {
			const webserv::uint16_t &port = it->first;
			const webserv::ServerConfigListType &serverConfigList = it->second;
			webserv::ServerRunningConfigListType runningConfigList;
			for (webserv::ServerConfigListType::const_iterator it = serverConfigList.begin();
					 it != serverConfigList.end();
					 ++it) {
				const webserv::ServerConfig &serverConfig = *it;
				runningConfigList.push_back(
					webserv::ServerRunningConfig(
						serverConfig,
						errorPageProvider,
						logger
					)
				);
			}
			webserv::ServerSocket *serverSocket = webserv::ServerSocket::createServerSocket(
				runningConfigList,
				port,
				logger
			);

			if (serverSocket == NULL) {
				LS_FATAL()
					<< "createServerSocket failed(NULL): port=" << port
					<< std::endl;
				for (PollableList::iterator it = pollableList.begin();
						 it != pollableList.end();
						 ++it) {
					delete *it;
					*it = NULL;
				}

				pollableList.clear();

				return 1;
			}
			pollableList.push_back(serverSocket);
		}
	} catch (const std::exception &e) {
		LS_FATAL()
			<< "createServerSocket failed(Exception)"
			<< " e.what(): " << e.what()
			<< std::endl;

		for (PollableList::iterator it = pollableList.begin();
				 it != pollableList.end();
				 ++it) {
			delete *it;
			*it = NULL;
		}

		pollableList.clear();

		return 1;
	}

	LS_INFO()
		<< "pollableList.size(): " << pollableList.size()
		<< std::endl;

	if (pollableList.empty()) {
		L_FATAL("pollableList.empty()");
		return 1;
	}

	webserv::Poll poll(pollableList, logger);
	while (!webserv::isExitSignalGot()) {
		try {
			bool result = poll.loop();
			if (!result) {
				L_FATAL("poll loop failed");
				return 1;
			}
		} catch (...) {
			L_FATAL("poll loop failed with uncaught exception");
			return 1;
		}
	}

	if (webserv::isExitSignalGot()) {
		L_INFO("exit signal got");
		return 1;
	}

	return 0;
}

static void __size_check()
{
	webserv::uint8_t uint8;
	webserv::uint16_t uint16;
	webserv::uint32_t uint32;

	if (sizeof(uint8) != 1) {
		std::cerr << "sizeof(uint8) != 1" << std::endl;
		std::exit(1);
	}

	if (sizeof(uint16) != 2) {
		std::cerr << "sizeof(uint16) != 2" << std::endl;
		std::exit(1);
	}

	if (sizeof(uint32) != 4) {
		std::cerr << "sizeof(uint32) != 4" << std::endl;
		std::exit(1);
	}
}
