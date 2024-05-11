#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#include <Logger.hpp>
#include <config/ServerRunningConfig.hpp>
#include <cstdio>
#include <iostream>
#include <poll/Poll.hpp>
#include <signal/signal_handler.hpp>
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

static webserv::ServerRunningConfigListType createDefaultServerConfigList(
	ushort port,
	webserv::Logger &logger
)
{
	webserv::HttpRouteConfig httpRouteConfig1;
	httpRouteConfig1.setDocumentRoot("./");
	httpRouteConfig1.setIsDocumentListingEnabled(true);
	httpRouteConfig1.setRequestPath("/");

	webserv::HttpRouteConfig httpRouteConfig2;
	httpRouteConfig2.setDocumentRoot("./srcs");
	httpRouteConfig2.setIsDocumentListingEnabled(false);
	httpRouteConfig2.setRequestPath("/route2");

	webserv::HttpRouteConfig httpRouteConfig3;
	httpRouteConfig3.setDocumentRoot("/");
	httpRouteConfig3.setIsDocumentListingEnabled(false);
	httpRouteConfig3.setRequestPath("/simple");

	webserv::RouteListType routeList;
	routeList.push_back(httpRouteConfig1);
	routeList.push_back(httpRouteConfig2);
	routeList.push_back(httpRouteConfig3);

	std::vector<std::string> hostNameList;
	hostNameList.push_back("localhost");
	webserv::ErrorPageMapType errorPageFileMap;
	errorPageFileMap[400] = "resources/sample1/400.html";
	errorPageFileMap[404] = "resources/sample1/404.html";
	webserv::ServerConfig serverConfig(
		hostNameList,
		port,
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
