#pragma once

#include <poll.h>

#define IS_POLLIN(revents) (((revents) & POLLIN) != 0)
#define IS_POLLOUT(revents) (((revents) & POLLOUT) != 0)

#define IS_POLLERR(revents) (((revents) & POLLERR) != 0)
#define IS_POLLHUP(revents) (((revents) & POLLHUP) != 0)
#define IS_POLLNVAL(revents) (((revents) & POLLNVAL) != 0)

#define IS_POLL_ANY_ERROR(revents) (IS_POLLERR(revents) || IS_POLLHUP(revents) || IS_POLLNVAL(revents))
