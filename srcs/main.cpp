#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

#include <Logger.hpp>
#include <cerrno>
#include <config/ServerRunningConfig.hpp>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <poll/Poll.hpp>
#include <signal/signal_handler.hpp>
#include <socket/ServerSocket.hpp>
#include <string>
#include <utils/getTimeStr.hpp>
#include <utils/to_string.hpp>

#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "utils/ErrorPageProvider.hpp"

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

static webserv::ServerRunningConfigListType createDefaultServerConfigList(
	ushort port,
	webserv::Logger &logger
)
{
	char pathBuf[MAXPATHLEN];

	webserv::HttpRouteConfig httpRouteConfig1;
	// Document Rootは絶対パスで指定する (YAML Parserで変換する)
	if (realpath("./", pathBuf) == NULL) {
		webserv::errno_t err = errno;
		LS_FATAL()
			<< "realpath() failed: " << std::strerror(err)
			<< std::endl;
		std::exit(1);
	}
	httpRouteConfig1.setDocumentRoot(pathBuf);
	httpRouteConfig1.setIsDocumentListingEnabled(true);
	httpRouteConfig1.setRequestPath("/");

	webserv::HttpRouteConfig httpRouteConfig2;
	if (realpath("./srcs", pathBuf) == NULL) {
		webserv::errno_t err = errno;
		LS_FATAL()
			<< "realpath() failed: " << std::strerror(err)
			<< std::endl;
		std::exit(1);
	}
	httpRouteConfig2.setDocumentRoot(pathBuf);
	httpRouteConfig2.setIsDocumentListingEnabled(false);
	httpRouteConfig2.setRequestPath("/route2");

	webserv::HttpRouteConfig httpRouteConfig3;
	httpRouteConfig3.setDocumentRoot("/");
	httpRouteConfig3.setIsDocumentListingEnabled(false);
	httpRouteConfig3.setRequestPath("/simple");

	// /resources
	webserv::HttpRouteConfig httpRouteConfigResources;
	if (realpath("./resources", pathBuf) == NULL) {
		webserv::errno_t err = errno;
		LS_FATAL()
			<< "realpath() failed: " << std::strerror(err)
			<< std::endl;
		std::exit(1);
	}
	httpRouteConfigResources.setDocumentRoot(pathBuf);
	httpRouteConfigResources.setIsDocumentListingEnabled(false);
	httpRouteConfigResources.setRequestPath("/resources");

	webserv::CgiConfig cgiConfigPhp;
	cgiConfigPhp.setExtensionWithoutDot("php");
	cgiConfigPhp.setCgiExecutableFullPath("/opt/homebrew/bin/php-cgi");
	webserv::env::EnvManager envManagerPhp;
	envManagerPhp.set("REDIRECT_STATUS", "200");
	cgiConfigPhp.setEnvPreset(envManagerPhp);

	webserv::CgiConfig cgiConfigSh;
	cgiConfigSh.setExtensionWithoutDot("sh");
	cgiConfigSh.setCgiExecutableFullPath("/bin/sh");

	webserv::CgiConfigListType cgiConfigListPhpSh;
	cgiConfigListPhpSh.push_back(cgiConfigPhp);
	cgiConfigListPhpSh.push_back(cgiConfigSh);
	httpRouteConfigResources.setCgiConfigList(cgiConfigListPhpSh);

	webserv::RouteListType routeList;
	routeList.push_back(httpRouteConfig1);
	routeList.push_back(httpRouteConfig2);
	routeList.push_back(httpRouteConfig3);
	routeList.push_back(httpRouteConfigResources);

	std::vector<std::string> hostNameList;
	hostNameList.push_back("localhost");
	webserv::ErrorPageMapType errorPageFileMap;
	errorPageFileMap[400] = "resources/sample1/400.html";
	errorPageFileMap[404] = "resources/sample1/404.html";
	webserv::ServerConfig serverConfig(
		hostNameList,
		port,
		100,
		// 128MB
		128 * 1024 * 1024,
		errorPageFileMap,
		routeList
	);

	webserv::utils::ErrorPageProvider errorPageProvider;
	webserv::ServerRunningConfigListType serverConfigList;
	serverConfigList.push_back(
		webserv::ServerRunningConfig(
			serverConfig,
			errorPageProvider,
			logger
		)
	);
	return serverConfigList;
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

int main(int argc, const char *argv[])
{
	__size_check();

	std::ofstream logFile;
	std::string logFilePath("./logs/webserv." + webserv::utils::getIso8601ShortTimeStr() + ".log");
	logFile.open(logFilePath.c_str(), std::ios_base::app);
	webserv::Logger logger(logFile);
	webserv::utils::ErrorPageProvider errorPageProvider;

	generatePidFile(argv[0], logger);

	std::cout << "Hello, World!" << std::endl;
	L_LOG("argv: " + get_argv_str(argc, argv));

	if (!webserv::registerSignalHandler()) {
		L_FATAL("registerSignalHandler failed");
		return 1;
	}

	std::vector<webserv::Pollable *> pollableList;
	webserv::ServerRunningConfigListType conf80 = createDefaultServerConfigList(80, logger);
	webserv::ServerSocket *serverSocket80 = webserv::ServerSocket::createServerSocket(
		conf80,
		80,
		logger
	);
	if (serverSocket80 == NULL) {
		L_FATAL("createServerSocket80 failed");
		return 1;
	}

	pollableList.push_back(serverSocket80);
	webserv::Poll poll(pollableList, logger);
	while (!webserv::isExitSignalGot()) {
		bool result = poll.loop();
		if (!result) {
			L_FATAL("poll loop failed");
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
