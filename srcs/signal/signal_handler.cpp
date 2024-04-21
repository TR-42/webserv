#include <signal/signal_handler.hpp>

namespace webserv
{

static sig_atomic_t _isExitSignalGot = false;

bool isExitSignalGot()
{
	return _isExitSignalGot;
}

static void signalHandler(
	int sig
)
{
	if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT) {
		_isExitSignalGot = true;
	}
}

bool registerSignalHandler()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGQUIT, signalHandler);

	return true;
}

}	 // namespace webserv
