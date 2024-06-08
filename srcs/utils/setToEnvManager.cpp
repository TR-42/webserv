#include <cctype>
#include <set>
#include <utils/setToEnvManager.hpp>

namespace webserv
{

namespace utils
{

static inline std::string toUpperSnakeCase(
	const std::string &str
)
{
	std::string upperStr(str);

	for (size_t i = 0; i < upperStr.size(); i++) {
		if (std::islower(upperStr[i])) {
			upperStr[i] = std::toupper(upperStr[i]);
		} else if (upperStr[i] == '-') {
			upperStr[i] = '_';
		}
	}

	return (upperStr);
}

static inline std::string getEnvKey(
	const std::string &key
)
{
	return ("HTTP_" + toUpperSnakeCase(key));
}

static const std::set<std::string> &getIgnoredHeaders()
{
	static std::set<std::string> IGNORED_HEADERS;

	if (!IGNORED_HEADERS.empty()) {
		return IGNORED_HEADERS;
	}

	IGNORED_HEADERS.insert("Authorization");
	IGNORED_HEADERS.insert("Content-Length");
	IGNORED_HEADERS.insert("Content-Type");

	return IGNORED_HEADERS;
}

void setToEnvManager(
	env::EnvManager &envManager,
	const HttpFieldMap &fieldMap
)
{
	static const std::set<std::string> &IGNORED_HEADERS = getIgnoredHeaders();

	for (
		HttpFieldMap::FieldMapType::const_iterator it = fieldMap.cbegin();
		it != fieldMap.cend();
		++it
	) {
		if (it->first.empty() || it->second.empty()) {
			continue;
		} else if (IGNORED_HEADERS.find(it->first) != IGNORED_HEADERS.end()) {
			continue;
		}

		const std::vector<std::string> &values = it->second;
		std::string combinedValue = values[0];
		for (size_t i = 1; i < values.size(); i++) {
			combinedValue += ", ";
			combinedValue += values[i];
		}
		envManager.set(
			getEnvKey(it->first),
			combinedValue
		);
	}
}

}	 // namespace utils

}	 // namespace webserv
