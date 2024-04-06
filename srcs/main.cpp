#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#include <Logger.hpp>
#include <cstdio>
#include <iostream>
#include <string>

#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

static std::string to_string(int i)
{
	char buf[16];
	snprintf(buf, sizeof(buf), "%d", i);
	return std::string(buf);
}
static std::string get_argv_str(int argc, const char *argv[])
{
	std::string str;
	for (int i = 0; i < argc; i++) {
		if (i != 0) {
			str += " ";
		}
		str += "[" + to_string(i) + "]:'" + argv[i] + "'";
	}
	return str;
}

int main(int argc, const char *argv[])
{
	webserv::Logger logger;

	std::cout << "Hello, World!" << std::endl;
	L_LOG("argv: " + get_argv_str(argc, argv));

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

	while (1) {
		struct sockaddr client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int client_fd = accept(fd, &client_addr, &client_addr_len);
		L_INFO("client_fd: " + to_string(client_fd));
		if (client_fd < 0) {
			perror("accept");
			close(fd);
			return 1;
		}
		LS_INFO()
			<< "client_addr: " << inet_ntoa(((struct sockaddr_in *)&client_addr)->sin_addr)
			<< ", "
			<< "client_port: " << ntohs(((struct sockaddr_in *)&client_addr)->sin_port)
			<< std::endl;
		size_t buf_size = 1024;
		char buf[buf_size];
		ssize_t recv_size = 0;
		// ssize_t recv_size = recv(client_fd, buf, buf_size, 0);

		webserv::HttpRequest request;
		while (request.isRequestHeaderParsed() == false &&
					 (recv_size = recv(client_fd, buf, buf_size, 0)) > 0) {
			std::string str(buf, recv_size);
			std::cout << "recv: " << str << std::endl;
			request.pushRequestRaw(std::vector<uint8_t>(buf, buf + recv_size));
		}
		if (recv_size < 0) {
			perror("recv");
			close(client_fd);
			close(fd);
			return 1;
		}

		webserv::HttpResponse response;
		std::string body = "Hello, World!\n";
		std::vector<uint8_t> body_vec(body.begin(), body.end());
		response.setVersion("HTTP/1.1");
		response.setStatusCode("200");
		response.setReasonPhrase("OK");
		response.setBody(body_vec);
		response.getHeaders()["Content-Type"].push_back("text/plain");
		response.getHeaders()["Content-Length"].push_back(to_string(body_vec.size()));

		std::vector<uint8_t> response_packet = response.generateResponsePacket();
		ssize_t send_size = send(client_fd, response_packet.data(), response_packet.size(), 0);
		L_INFO("send_size: " + to_string(send_size));
		if (send_size < 0) {
			perror("send");
			close(client_fd);
			close(fd);
			return 1;
		}

		close(client_fd);
	}

	return 0;
}
