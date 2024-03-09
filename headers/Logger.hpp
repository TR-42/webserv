#pragma once

#include <string>

#define LOGGER_FUNC_DECL(name) \
	void name( \
		std::string const &message, \
		const char *file = __builtin_FILE(), \
		int line = __builtin_LINE(), \
		const char *func = __builtin_FUNCTION() \
	)

#define DO_LOG(logger, level, message) \
	logger.level(message, __FILE__, __LINE__, __PRETTY_FUNCTION__)

/**
 * @brief 変数を指定してログ出力を行う (レベル: log)
 */
#define V_LOG(logger, message) DO_LOG(logger, log, message)
/**
 * @brief 変数を指定してログ出力を行う (レベル: debug)
 */
#define V_DEBUG(logger, message) DO_LOG(logger, debug, message)
/**
 * @brief 変数を指定してログ出力を行う (レベル: info)
 */
#define V_INFO(logger, message) DO_LOG(logger, info, message)
/**
 * @brief 変数を指定してログ出力を行う (レベル: warn)
 */
#define V_WARN(logger, message) DO_LOG(logger, warn, message)
/**
 * @brief 変数を指定してログ出力を行う (レベル: error)
 */
#define V_ERROR(logger, message) DO_LOG(logger, error, message)
/**
 * @brief 変数を指定してログ出力を行う (レベル: fatal)
 */
#define V_FATAL(logger, message) DO_LOG(logger, fatal, message)

/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: log)
 */
#define C_LOG(message) V_LOG(this->logger, message)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: debug)
 */
#define C_DEBUG(message) V_DEBUG(this->logger, message)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: info)
 */
#define C_INFO(message) V_INFO(this->logger, message)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: warn)
 */
#define C_WARN(message) V_WARN(this->logger, message)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: error)
 */
#define C_ERROR(message) V_ERROR(this->logger, message)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: fatal)
 */
#define C_FATAL(message) V_FATAL(this->logger, message)

/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: log)
 */
#define L_LOG(message) V_LOG(logger, message)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: debug)
 */
#define L_DEBUG(message) V_DEBUG(logger, message)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: info)
 */
#define L_INFO(message) V_INFO(logger, message)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: warn)
 */
#define L_WARN(message) V_WARN(logger, message)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: error)
 */
#define L_ERROR(message) V_ERROR(logger, message)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: fatal)
 */
#define L_FATAL(message) V_FATAL(logger, message)

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
	);

 public:
	Logger();
	Logger(std::ostream &os);
	~Logger();

	LOGGER_FUNC_DECL(log);
	LOGGER_FUNC_DECL(debug);
	LOGGER_FUNC_DECL(info);
	LOGGER_FUNC_DECL(warn);
	LOGGER_FUNC_DECL(error);
	LOGGER_FUNC_DECL(fatal);
};

}	 // namespace webserv

#undef LOGGER_FUNC_DECL
