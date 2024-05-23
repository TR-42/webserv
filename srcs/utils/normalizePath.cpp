#include <utils/normalizePath.hpp>
#include <vector>

namespace webserv
{

namespace utils
{

static std::vector<std::string> _getPathSegmentList(
	const std::string &path
)
{
	const size_t pathSize = path.size();
	std::vector<std::string> pathSegmentList;

	for (size_t i = 0; i < pathSize; ++i) {
		if (path[i] == '/') {
			continue;
		}

		size_t nextSlashPos = path.find('/', i);
		if (nextSlashPos == std::string::npos) {
			nextSlashPos = pathSize;
		}

		std::string segment = path.substr(i, nextSlashPos - i);
		if (segment == "..") {
			if (!pathSegmentList.empty()) {
				pathSegmentList.pop_back();
			}
		} else if (segment != ".") {
			pathSegmentList.push_back(segment);
		}

		i = nextSlashPos;
	}

	return pathSegmentList;
}

static size_t _getResultLength(
	const std::vector<std::string> &pathSegmentList,
	bool isAbsolutePath,
	bool isEndWithSlash
)
{
	size_t resultSize = 0;

	if (isAbsolutePath) {
		++resultSize;
	}

	const size_t pathSegmentListSize = pathSegmentList.size();
	// pathSegmentが存在しない場合、directoryを表すslashを付けようがない
	if (pathSegmentListSize == 0) {
		return resultSize;
	}

	for (size_t i = 0; i < pathSegmentListSize; ++i) {
		if (i != 0) {
			++resultSize;
		}
		resultSize += pathSegmentList[i].size();
	}

	if (isEndWithSlash) {
		++resultSize;
	}

	return resultSize;
}

static std::string _joinPathSegmentList(
	const std::vector<std::string> &pathSegmentList,
	bool isAbsolutePath,
	bool isEndWithSlash
)
{
	const size_t resultSize = _getResultLength(pathSegmentList, isAbsolutePath, isEndWithSlash);
	std::string result;
	result.reserve(resultSize);

	if (isAbsolutePath) {
		result += '/';
	}

	const size_t pathSegmentListSize = pathSegmentList.size();
	if (pathSegmentListSize == 0) {
		return result;
	}

	for (size_t i = 0; i < pathSegmentListSize; ++i) {
		if (i != 0) {
			result += '/';
		}
		result += pathSegmentList[i];
	}

	if (isEndWithSlash) {
		result += '/';
	}

	return result;
}

std::string normalizePath(
	const std::string &path
)
{
	if (path.empty()) {
		return path;
	}

	bool isAbsolutePath = path[0] == '/';
	bool isEndWithSlash = path[path.size() - 1] == '/';

	return _joinPathSegmentList(
		_getPathSegmentList(path),
		isAbsolutePath,
		isEndWithSlash
	);
}

}	 // namespace utils

}	 // namespace webserv
