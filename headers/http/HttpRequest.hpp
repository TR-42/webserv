#pragma once

#include <Logger.hpp>
#include <map>
#include <string>
#include <vector>

namespace webserv
{

class HttpRequest
{
 public:
	//  private:
	std::string _Method;
	std::string _Path;
	std::string _Version;
	std::map<std::string, std::vector<std::string> > _Headers;
	std::vector<uint8_t> _Body;

	bool _IsRequestLineParsed;
	bool _IsRequestHeaderParsed;

	std::vector<uint8_t> _UnparsedRequestRaw;
	// chunkは後で実装

 public:
	HttpRequest();
	/**
	 * @brief 今までに受け取ったパケットとともに、HTTPリクエストを解析する
	 *
	 * @param requestRaw 受け取ったパケット
	 * @return true 特に問題なく処理が終了した
	 * @return false リクエストにエラーがあった
	 */
	bool pushRequestRaw(
		const std::vector<uint8_t> &requestRaw
	);

 private:
	bool parseRequestLine(
		const std::vector<uint8_t> &requestRawLine
	);

	bool parseRequestHeader(
		const std::vector<uint8_t> &requestRawLine
	);
	Logger logger;
};

}	 // namespace webserv
