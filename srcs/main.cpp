#include <Logger.hpp>
#include <iostream>

class mainClass
{
 private:
	webserv::Logger logger;

 public:
	void testLogger(void);
};

void mainClass::testLogger()
{
	C_LOG("Hello, World!");
	C_DEBUG("Hello, World!");
	C_INFO("Hello, World!");
	C_WARN("Hello, World!");
	C_ERROR("Hello, World!");
	C_FATAL("Hello, World!");
}

int main(void)
{
	webserv::Logger logger;

	std::cout << "Hello, World!" << std::endl;
	L_LOG("Hello, World!");
	L_DEBUG("Hello, World!");
	L_INFO("Hello, World!");
	L_WARN("Hello, World!");
	L_ERROR("Hello, World!");
	L_FATAL("Hello, World!");
	mainClass().testLogger();
	return 0;
}

#ifdef DEBUG

// - getpid
// - system
#include <unistd.h>

// - getenv
#include <stdlib.h>

// - sprintf
#include <stdio.h>

#define DEBUG_LEAKS_CMD_LEN (64)

extern "C" {

__attribute__((destructor)) static void destructor(void)
{
	char cmd[DEBUG_LEAKS_CMD_LEN];

	snprintf(cmd, DEBUG_LEAKS_CMD_LEN, "leaks -quiet %d > /dev/stderr", getpid());
	system(cmd);
}
}

#endif
