#pragma once

#include <poll.h>

#define IS_POLLIN(revents) ((revents) & POLLIN)
#define IS_POLLOUT(revents) ((revents) & POLLOUT)
