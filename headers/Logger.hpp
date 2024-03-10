#pragma once

#include <string>

#define LOGGER_FUNC_DECL(name) \
	void name( \
		const char *file, \
		int line, \
		const char *func, \
		std::string const &message \
	); \
	std::ostream &name( \
		const char *file, \
		int line, \
		const char *func \
	);

// ##__VA_ARGS__ ref: https://tyfkda.github.io/blog/2015/03/04/va_args.html
// 引数が0個の場合にコンパイルエラーになるのを防ぐために##を使う
#define DO_LOG(logger, level, ...) \
	logger.level(__FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)

/**
 * @brief 変数を指定してログ出力を行う (レベル: log)
 */
#define V_LOG(logger, ...) DO_LOG(logger, log, ##__VA_ARGS__)
/**
 * @brief 変数を指定してログ出力を行う (レベル: debug)
 */
#define V_DEBUG(logger, ...) DO_LOG(logger, debug, ##__VA_ARGS__)
/**
 * @brief 変数を指定してログ出力を行う (レベル: info)
 */
#define V_INFO(logger, ...) DO_LOG(logger, info, ##__VA_ARGS__)
/**
 * @brief 変数を指定してログ出力を行う (レベル: warn)
 */
#define V_WARN(logger, ...) DO_LOG(logger, warn, ##__VA_ARGS__)
/**
 * @brief 変数を指定してログ出力を行う (レベル: error)
 */
#define V_ERROR(logger, ...) DO_LOG(logger, error, ##__VA_ARGS__)
/**
 * @brief 変数を指定してログ出力を行う (レベル: fatal)
 */
#define V_FATAL(logger, ...) DO_LOG(logger, fatal, ##__VA_ARGS__)

/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: log)
 */
#define C_LOG(...) V_LOG(this->logger, ##__VA_ARGS__)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: debug)
 */
#define C_DEBUG(...) V_DEBUG(this->logger, ##__VA_ARGS__)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: info)
 */
#define C_INFO(...) V_INFO(this->logger, ##__VA_ARGS__)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: warn)
 */
#define C_WARN(...) V_WARN(this->logger, ##__VA_ARGS__)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: error)
 */
#define C_ERROR(...) V_ERROR(this->logger, ##__VA_ARGS__)
/**
 * @brief クラスのメンバ変数として`logger`を持つ場合用のログ出力マクロ (レベル: fatal)
 */
#define C_FATAL(...) V_FATAL(this->logger, ##__VA_ARGS__)

/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: log)
 */
#define L_LOG(...) V_LOG(logger, ##__VA_ARGS__)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: debug)
 */
#define L_DEBUG(...) V_DEBUG(logger, ##__VA_ARGS__)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: info)
 */
#define L_INFO(...) V_INFO(logger, ##__VA_ARGS__)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: warn)
 */
#define L_WARN(...) V_WARN(logger, ##__VA_ARGS__)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: error)
 */
#define L_ERROR(...) V_ERROR(logger, ##__VA_ARGS__)
/**
 * @brief 現在のスコープに `logger` を持つ場合用のログ出力マクロ (レベル: fatal)
 */
#define L_FATAL(...) V_FATAL(logger, ##__VA_ARGS__)

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
	std::ostream &_print(
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
