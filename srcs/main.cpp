#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#include <Logger.hpp>
#include <cstdio>
#include <iostream>
#include <signal/signal_handler.hpp>
#include <socket/Poll.hpp>
#include <socket/ServerSocket.hpp>
#include <string>

#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "utils.hpp"
#include "utils/ErrorPageProvider.hpp"

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

static webserv::ServerConfigListType createDefaultServerConfigList(
	ushort port = 80
)
{
	webserv::HttpRouteConfig httpRouteConfig;

	webserv::RouteListType routeList;
	routeList.push_back(httpRouteConfig);

	std::vector<std::string> hostNameList;
	hostNameList.push_back("localhost");
	webserv::ServerConfig serverConfig(
		hostNameList,
		"localhost",
		port,
		// 128MB
		128 * 1024 * 1024,
		webserv::ErrorPageMapType(),
		routeList
	);

	webserv::ServerConfigListType serverConfigList;
	serverConfigList.push_back(serverConfig);
	return serverConfigList;
}

int main(int argc, const char *argv[])
{
	webserv::Logger logger;
	webserv::utils::ErrorPageProvider errorPageProvider;

	std::cout << "Hello, World!" << std::endl;
	L_LOG("argv: " + get_argv_str(argc, argv));

	if (!webserv::registerSignalHandler()) {
		L_FATAL("registerSignalHandler failed");
		return 1;
	}

	std::vector<webserv::Socket *> socketList;
	webserv::ServerSocket *serverSocket80 = webserv::ServerSocket::createServerSocket(
		createDefaultServerConfigList(),
		logger
	);
	if (serverSocket80 == NULL) {
		L_FATAL("createServerSocket80 failed");
		return 1;
	}
	webserv::ServerSocket *serverSocket81 = webserv::ServerSocket::createServerSocket(
		createDefaultServerConfigList(81),
		logger
	);
	if (serverSocket81 == NULL) {
		L_FATAL("createServerSocket81 failed");
		delete serverSocket80;
		return 1;
	}
	webserv::ServerSocket *serverSocket82 = webserv::ServerSocket::createServerSocket(
		createDefaultServerConfigList(82),
		logger
	);
	if (serverSocket82 == NULL) {
		L_FATAL("createServerSocket82 failed");
		delete serverSocket80;
		delete serverSocket81;
		return 1;
	}

	socketList.push_back(serverSocket80);
	socketList.push_back(serverSocket81);
	socketList.push_back(serverSocket82);
	webserv::Poll poll(socketList, logger);
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
