#include <Logger.hpp>
#include <ctime>
#include <iostream>

#define FMT_RESET "\033[0m"
#define FMT_BOLD "\033[1m"
#define FMT_DIM "\033[2m"
#define FMT_UNDERLINE "\033[4m"
#define FMT_BLINK "\033[5m"
#define FMT_REVERSE "\033[7m"
#define FMT_HIDDEN "\033[8m"

#define FMT_FG_BLACK "\033[30m"
#define FMT_FG_RED "\033[31m"
#define FMT_FG_GREEN "\033[32m"
#define FMT_FG_YELLOW "\033[33m"
#define FMT_FG_BLUE "\033[34m"
#define FMT_FG_MAGENTA "\033[35m"
#define FMT_FG_CYAN "\033[36m"
#define FMT_FG_WHITE "\033[37m"

#define FMT_BG_BLACK "\033[40m"
#define FMT_BG_RED "\033[41m"
#define FMT_BG_GREEN "\033[42m"
#define FMT_BG_YELLOW "\033[43m"
#define FMT_BG_BLUE "\033[44m"
#define FMT_BG_MAGENTA "\033[45m"
#define FMT_BG_CYAN "\033[46m"
#define FMT_BG_WHITE "\033[47m"

#define LEVEL_log FMT_DIM "  LOG" FMT_RESET
#define LEVEL_debug "DEBUG"
#define LEVEL_info FMT_FG_CYAN " INFO" FMT_RESET
#define LEVEL_warn FMT_FG_YELLOW FMT_BG_BLACK " WARN" FMT_RESET
#define LEVEL_error FMT_FG_RED FMT_BG_BLACK "ERROR" FMT_RESET
#define LEVEL_fatal FMT_BOLD FMT_FG_RED FMT_BG_BLACK "FATAL" FMT_RESET

#define DEFAULT_TIMESTAMP_VALUE_BODY "1970-01-01T00:00:00"
#define DEFAULT_TIMESTAMP_VALUE_Z "Z"
#ifdef DEBUG
#define TIMESTAMP_NS_FORMAT ".%09ld" DEFAULT_TIMESTAMP_VALUE_Z
#define DEFAULT_TIMESTAMP_VALUE_NS ".000000000"
#else
#define DEFAULT_TIMESTAMP_VALUE_NS ""
#endif
#define DEFAULT_TIMESTAMP_VALUE_DEF ( \
	DEFAULT_TIMESTAMP_VALUE_BODY \
		DEFAULT_TIMESTAMP_VALUE_NS \
			DEFAULT_TIMESTAMP_VALUE_Z \
)

static const std::string
_get_timestamp()
{
	static const std::string DEFAULT_TIMESTAMP_VALUE = DEFAULT_TIMESTAMP_VALUE_DEF;
#ifdef DEBUG
	struct timespec ts;
	const int ts_result = clock_gettime(CLOCK_REALTIME, &ts);
	if (ts_result == -1) {
		return DEFAULT_TIMESTAMP_VALUE;
	}
	const std::time_t t = ts.tv_sec;
#else
	const std::time_t t = std::time(NULL);
#endif
	if (t == -1) {
		return DEFAULT_TIMESTAMP_VALUE;
	}

	const std::tm tm = *std::gmtime(&t);
	char buf[sizeof(DEFAULT_TIMESTAMP_VALUE_DEF)];
	size_t tm_body_len = std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
#ifdef DEBUG
	std::snprintf(buf + tm_body_len, sizeof(buf) - tm_body_len, TIMESTAMP_NS_FORMAT, ts.tv_nsec);
#else
	std::strcat(buf + tm_body_len, DEFAULT_TIMESTAMP_VALUE_Z);
#endif

	return std::string(buf);
}

void webserv::Logger::_print(
	std::string const &message,
	std::string const &level,
	const char *file,
	int line,
	const char *func
)
{
	this->_os
		<< _get_timestamp()
		<< " " << file << ":" << line
		<< "\t" << func
		<< "\t[" << level << "]"
		<< '\t' << message
		<< std::endl;
}

webserv::Logger::Logger() : _os(std::cout)
{
}

webserv::Logger::Logger(std::ostream &os) : _os(os)
{
}

webserv::Logger::~Logger()
{
}

#define LOGGER_FUNC_IMPL(name) \
	void webserv::Logger::name( \
		std::string const &message, \
		const char *file, \
		int line, \
		const char *func \
	) \
	{ \
		this->_print(message, LEVEL_##name, file, line, func); \
	}

LOGGER_FUNC_IMPL(log);
LOGGER_FUNC_IMPL(debug);
LOGGER_FUNC_IMPL(info);
LOGGER_FUNC_IMPL(warn);
LOGGER_FUNC_IMPL(error);
LOGGER_FUNC_IMPL(fatal);
