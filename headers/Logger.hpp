#pragma once

#include <string>

#define LOGGER_FUNC_DECL(name) \
	void name( \
		const char *file, \
		int line, \
		const char *func, \
		std::string const &message \
	) const; \
	std::ostream &name( \
		const char *file, \
		int line, \
		const char *func \
	) const;

// ##__VA_ARGS__ ref: https://tyfkda.github.io/blog/2015/03/04/va_args.html
#define DO_LOG(logger, level, ...) \
	logger.level(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
#define DO_S_LOG(logger, level) \
	logger.level(__FILE__, __LINE__, __PRETTY_FUNCTION__)

/**
 * @brief 変数を指定してログ出力を行う (レベル: log)
 */
#define V_LOG(logger, ...) DO_LOG(logger, log, ##__VA_ARGS__)
#define VS_LOG(logger) DO_S_LOG(logger, log)
/**
 * @brief 変数を指定してログ出力を行う (レベル: debug)
 */
#define V_DEBUG(logger, ...) DO_LOG(logger, debug, ##__VA_ARGS__)
#define VS_DEBUG(logger) DO_S_LOG(logger, debug)
/**
 * @brief 変数を指定してログ出力を行う (レベル: info)
 */
#define V_INFO(logger, ...) DO_LOG(logger, info, ##__VA_ARGS__)
#define VS_INFO(logger) DO_S_LOG(logger, info)
/**
 * @brief 変数を指定してログ出力を行う (レベル: warn)
 */
#define V_WARN(logger, ...) DO_LOG(logger, warn, ##__VA_ARGS__)
#define VS_WARN(logger) DO_LOG(logger, warn)
/**
 * @brief 変数を指定してログ出力を行う (レベル: error)
 */
#define V_ERROR(logger, ...) DO_LOG(logger, error, ##__VA_ARGS__)
#define VS_ERROR(logger) DO_S_LOG(logger, error)
/**
 * @brief 変数を指定してログ出力を行う (レベル: fatal)
 */
#define V_FATAL(logger, ...) DO_LOG(logger, fatal, ##__VA_ARGS__)
#define VS_FATAL(logger) DO_S_LOG(logger, fatal)

/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: log)
 */
#define C_LOG(...) V_LOG(this->logger, ##__VA_ARGS__)
#define CS_LOG() VS_LOG(this->logger)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: debug)
 */
#define C_DEBUG(...) V_DEBUG(this->logger, ##__VA_ARGS__)
#define CS_DEBUG() VS_DEBUG(this->logger)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: info)
 */
#define C_INFO(...) V_INFO(this->logger, ##__VA_ARGS__)
#define CS_INFO() VS_INFO(this->logger)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: warn)
 */
#define C_WARN(...) V_WARN(this->logger, ##__VA_ARGS__)
#define CS_WARN() VS_WARN(this->logger)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: error)
 */
#define C_ERROR(...) V_ERROR(this->logger, ##__VA_ARGS__)
#define CS_ERROR() VS_ERROR(this->logger)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: fatal)
 */
#define C_FATAL(...) V_FATAL(this->logger, ##__VA_ARGS__)
#define CS_FATAL() VS_FATAL(this->logger)

/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: log)
 */
#define L_LOG(...) V_LOG(logger, ##__VA_ARGS__)
#define LS_LOG() VS_LOG(logger)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: debug)
 */
#define L_DEBUG(...) V_DEBUG(logger, ##__VA_ARGS__)
#define LS_DEBUG() VS_DEBUG(logger)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: info)
 */
#define L_INFO(...) V_INFO(logger, ##__VA_ARGS__)
#define LS_INFO() VS_INFO(logger)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: warn)
 */
#define L_WARN(...) V_WARN(logger, ##__VA_ARGS__)
#define LS_WARN() VS_WARN(logger)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: error)
 */
#define L_ERROR(...) V_ERROR(logger, ##__VA_ARGS__)
#define LS_ERROR() VS_ERROR(logger)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: fatal)
 */
#define L_FATAL(...) V_FATAL(logger, ##__VA_ARGS__)
#define LS_FATAL() VS_FATAL(logger)

namespace webserv
{

class Logger
{
 private:
	std::string _class;
	std::ostream &_os;

	void _print(
		std::string const &message,
		std::string const &level,
		const char *file,
		int line,
		const char *func
	) const;
	std::ostream &_print(
		std::string const &level,
		const char *file,
		int line,
		const char *func
	) const;

 public:
	Logger();
	Logger(std::ostream &os);

	LOGGER_FUNC_DECL(log);
	LOGGER_FUNC_DECL(debug);
	LOGGER_FUNC_DECL(info);
	LOGGER_FUNC_DECL(warn);
	LOGGER_FUNC_DECL(error);
	LOGGER_FUNC_DECL(fatal);
};

}	 // namespace webserv

#undef LOGGER_FUNC_DECL
