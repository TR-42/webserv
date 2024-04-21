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

	struct sockaddr_in addr;
	uint16_t port = 80;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd < 0) {
		perror("socket");
		return 1;
	}
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		close(fd);
		return 1;
	}
	if (listen(fd, 10) < 0) {
		perror("listen");
		close(fd);
		return 1;
	}

	std::vector<webserv::Socket *> socketList;
	socketList.push_back(new webserv::ServerSocket(fd, logger));
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
