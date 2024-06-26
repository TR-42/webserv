#include <Logger.hpp>
#include <cstring>
#include <ctime>
#include <iostream>
#include <stdexcept>

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
#define TIMESTAMP_US_FORMAT ".%06ld" DEFAULT_TIMESTAMP_VALUE_Z
#define DEFAULT_TIMESTAMP_VALUE_US ".000000"
#define DEFAULT_TIMESTAMP_VALUE_DEF ( \
	DEFAULT_TIMESTAMP_VALUE_BODY \
		DEFAULT_TIMESTAMP_VALUE_US \
			DEFAULT_TIMESTAMP_VALUE_Z \
)
#define NS_TO_US(ns) ((ns) / 1000)

static void _set_us_str(
	char *buf,
	long ns
)
{
	long us = NS_TO_US(ns);
	buf[0] = '.';
	for (int i = 0; i < 6; i++) {
		buf[6 - i] = '0' + (us % 10);
		us /= 10;
	}
}

static const char *
_get_timestamp()
{
	static const char DEFAULT_TIMESTAMP_VALUE[] = DEFAULT_TIMESTAMP_VALUE_DEF;
	struct timespec ts;

	// 使用可能関数に含まれる
	// ref: https://discord.com/channels/691903146909237289/691908977918738453/1219175088461709312
	const int ts_result = clock_gettime(CLOCK_REALTIME, &ts);
	if (ts_result == -1) {
		return DEFAULT_TIMESTAMP_VALUE;
	}
	const std::time_t t = ts.tv_sec;
	if (t == -1) {
		return DEFAULT_TIMESTAMP_VALUE;
	}

	const std::tm tm = *std::gmtime(&t);
	static char buf[sizeof(DEFAULT_TIMESTAMP_VALUE_DEF)];
	size_t tm_body_len = std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
	_set_us_str(buf + tm_body_len, ts.tv_nsec);
	buf[sizeof(DEFAULT_TIMESTAMP_VALUE_DEF) - 2] = 'Z';
	buf[sizeof(DEFAULT_TIMESTAMP_VALUE_DEF) - 1] = '\0';

	return buf;
}

static const std::string _getCustomIdWithSpace(
	const std::string &CustomId
)
{
	return CustomId.empty() ? CustomId : " `" + CustomId + "`";
}

void webserv::Logger::_print(
	std::string const &message,
	std::string const &level,
	const char *file,
	int line,
	const char *func
) const
{
	this->_print(level, file, line, func)
		<< message
		<< std::endl;
}

std::ostream &webserv::Logger::_print(
	std::string const &level,
	const char *file,
	int line,
	const char *func
) const
{
	return (
		this->_os
		<< _get_timestamp()
		<< this->_CustomIdWithSpace
		<< " " << file << ":" << line
		<< "\t" << func
		<< "\t[" << level << "]"
		<< '\t'
	);
}

webserv::Logger::Logger(
) : _CustomId(),
		_CustomIdWithSpace(),
		_os(std::clog)
{
}

webserv::Logger::Logger(
	const std::string &CustomId
) : _CustomId(CustomId),
		_CustomIdWithSpace(_getCustomIdWithSpace(CustomId)),
		_os(std::clog)
{
}

webserv::Logger::Logger(
	std::ostream &os
) : _CustomId(),
		_CustomIdWithSpace(),
		_os(os)
{
}

webserv::Logger::Logger(
	std::ostream &os,
	const std::string &CustomId
) : _CustomId(CustomId),
		_CustomIdWithSpace(_getCustomIdWithSpace(CustomId)),
		_os(os)
{
}

webserv::Logger::Logger(
	const Logger &src
) : _CustomId(src._CustomId),
		_CustomIdWithSpace(src._CustomIdWithSpace),
		_os(src._os)
{
}

webserv::Logger::Logger(
	const Logger &src,
	const std::string &CustomId
) : _CustomId(CustomId),
		_CustomIdWithSpace(_getCustomIdWithSpace(CustomId)),
		_os(src._os)
{
}

std::string webserv::Logger::getCustomId() const
{
	return this->_CustomId;
}

#define LOGGER_FUNC_IMPL(name) \
	void webserv::Logger::name( \
		const char *file, \
		int line, \
		const char *func, \
		std::string const &message \
	) const \
	{ \
		this->_print(message, LEVEL_##name, file, line, func); \
	}; \
	std::ostream &webserv::Logger::name( \
		const char *file, \
		int line, \
		const char *func \
	) const \
	{ \
		return this->_print(LEVEL_##name, file, line, func); \
	};

LOGGER_FUNC_IMPL(log);
LOGGER_FUNC_IMPL(debug);
LOGGER_FUNC_IMPL(info);
LOGGER_FUNC_IMPL(warn);
LOGGER_FUNC_IMPL(error);
LOGGER_FUNC_IMPL(fatal);
