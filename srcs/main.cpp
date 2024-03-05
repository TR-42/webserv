#include <iostream>

int main(void) {
    std::cout << "Hello, World!" << std::endl;
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
