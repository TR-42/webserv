#include <http/HttpRequest.hpp>

bool webserv::HttpRequest::pushRequestRaw(
	const std::vector<uint8_t> &requestRaw
)
{
	size_t newlinePos;
	size_t requestRawSize = requestRaw.size();
	if (requestRawSize == 0) {
		return true;
	}
	_UnparsedRequestRaw.insert(_UnparsedRequestRaw.end(), requestRaw.begin(), requestRaw.end());
	for (newlinePos = 0; newlinePos < requestRawSize; ++newlinePos) {
		if (requestRaw[newlinePos] == '\n') {
			break;
		}
	}
	if (newlinePos == requestRawSize) {
		return true;
	}

	if (_IsRequestHeaderParsed == false) {
		// リクエストラインの解析
		if (_IsRequestLineParsed == false) {
			const uint8_t *requestRawData = _UnparsedRequestRaw.data();
			const uint8_t *spacePos1 = (const uint8_t *)std::memchr(requestRawData, ' ', newlinePos);
			if (spacePos1 == NULL) {
				return false;
			}
			size_t lenToSpacePos1 = spacePos1 - requestRawData;
			if (lenToSpacePos1 == 0) {
				return false;
			}
			_Method = std::string((const char *)requestRawData, lenToSpacePos1);
			const uint8_t *pathSegment = spacePos1 + 1;
			const uint8_t *spacePos2 = (const uint8_t *)std::memchr(pathSegment, ' ', newlinePos - lenToSpacePos1);
			if (spacePos2 == NULL) {
				return false;
			}
			size_t lenToSpacePos2 = spacePos2 - pathSegment;
			if (lenToSpacePos2 == 0) {
				return false;
			}
			_Path = std::string((const char *)pathSegment, lenToSpacePos2);
			const uint8_t *versionSegment = spacePos2 + 1;
			size_t versionStringLength = newlinePos - lenToSpacePos2 - lenToSpacePos1 - 2;
			if (std::memchr(versionSegment, ' ', versionStringLength) != NULL) {
				return false;
			}
			if (versionStringLength == 0) {
				return false;
			}
			if (requestRaw[newlinePos - 1] == '\r') {
				--versionStringLength;
			}
			_Version = std::string((const char *)versionSegment, versionStringLength);
			_IsRequestLineParsed = true;
			_UnparsedRequestRaw.erase(_UnparsedRequestRaw.begin(), _UnparsedRequestRaw.begin() + newlinePos + 1);
			// 	} else {
			// 		// ヘッダの解析
			// 		size_t colonPos;
			// 		for (colonPos = 0; colonPos < newlinePos; ++colonPos) {
			// 			if (requestRaw[colonPos] == ':') {
			// 				break;
			// 			}
			// 		}
			// 		if (colonPos == newlinePos) {
			// 			_IsRequestHeaderParsed = true;
			// 		} else {
			// 			std::string key = std::string(requestRaw.begin(), requestRaw.begin() + colonPos);
			// 			std::string value = std::string(requestRaw.begin() + colonPos + 1, requestRaw.begin() + newlinePos - 1);
			// 			_Headers[key].push_back(value);
			// 		}
			// 	}
			// } else {
			// 	// ボディの解析
			// 	_Body.insert(_Body.end(), requestRaw.begin(), requestRaw.end());
		}
	}
	return true;
}
