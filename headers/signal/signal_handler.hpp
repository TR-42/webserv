#pragma once

#include <signal.h>

namespace webserv
{

bool isExitSignalGot();

bool registerSignalHandler();

}	 // namespace webserv
