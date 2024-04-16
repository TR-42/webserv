#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

#include <Logger.hpp>
#include <cstdio>
#include <iostream>
#include <string>

#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "utils.hpp"
#include "utils/ErrorPageProvider.hpp"

bool process_http_request(
	int client_fd,
	const webserv::Logger &logger,
	const webserv::utils::ErrorPageProvider &errorPageProvider
);
bool send_http_response(
	int client_fd,
	const webserv::HttpResponse &response
);
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
		L_INFO("client_fd: " + webserv::utils::to_string(client_fd));
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
		bool request_result = process_http_request(client_fd, logger, errorPageProvider);
		close(client_fd);
		if (request_result == false) {
			close(fd);
			return 1;
		}
	}

	return 0;
}

bool process_http_request(
	int client_fd,
	const webserv::Logger &logger,
	const webserv::utils::ErrorPageProvider &errorPageProvider
)
{
	size_t buf_size = 1024;
	char buf[buf_size];
	ssize_t recv_size = 0;

	webserv::HttpRequest request;
	while (request.isRequestBodyLengthEnough() == false &&
				 (recv_size = recv(client_fd, buf, buf_size, 0)) > 0) {
		std::string str(buf, recv_size);
		std::cout << "recv: " << str << std::endl;
		if (request.pushRequestRaw(std::vector<uint8_t>(buf, buf + recv_size)) == false) {
			L_WARN("request pushRequestRaw failed");
			return (send_http_response(client_fd, errorPageProvider.badRequest()));
		}
	}
	if (recv_size < 0) {
		perror("recv");
		return false;
	} else if (request.isRequestBodyLengthTooMuch()) {
		L_WARN("request body too much");
		return (send_http_response(client_fd, errorPageProvider.badRequest()));
	}

	if (request.getPath().empty() || request.getPath()[0] != '/') {
		L_WARN("request path is empty or not start with '/'");
		return (send_http_response(client_fd, errorPageProvider.badRequest()));
	}
	std::string path = request.getPath().substr(1);
	L_INFO("path: " + path);
	webserv::HttpResponse response = errorPageProvider.getErrorPage(path);
	LS_LOG()
		<< "response: "
		<< "version: " << response.getVersion()
		<< ", "
		<< "statusCode: " << response.getStatusCode()
		<< ", "
		<< "reasonPhrase: " << response.getReasonPhrase()
		<< ", "
		// << "body: " << response.getBody()
		<< std::endl;
	return (send_http_response(client_fd, response));
}

bool send_http_response(
	int client_fd,
	const webserv::HttpResponse &response
)
{
	std::vector<uint8_t> response_packet = response.generateResponsePacket();
	ssize_t send_size = send(client_fd, response_packet.data(), response_packet.size(), 0);
	if (send_size < 0) {
		perror("send");
		return false;
	}
	return true;
}
