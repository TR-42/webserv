#include <sys/wait.h>

#include <utils/to_string.hpp>
#include <utils/waitResultStatusToString.hpp>

namespace webserv
{

namespace utils
{
std::string waitResultStatusToString(
	int status
)
{
	if (WIFEXITED(status)) {
		return "exit status: " + utils::to_string(WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
		return "signal: " + utils::to_string(WTERMSIG(status));
	} else {
		return "unknown status";
	}
}

}	 // namespace utils

}	 // namespace webserv
