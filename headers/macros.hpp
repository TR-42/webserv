#pragma once

#include <poll.h>

#define IS_POLLIN(revents) ((revents) & POLLIN)
#define IS_POLLOUT(revents) ((revents) & POLLOUT)

#define IS_POLLERR(revents) ((revents) & POLLERR)
#define IS_POLLHUP(revents) ((revents) & POLLHUP)
#define IS_POLLNVAL(revents) ((revents) & POLLNVAL)
