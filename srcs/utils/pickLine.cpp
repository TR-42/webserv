#include <utils/pickLine.hpp>

namespace webserv
{

namespace utils
{

std::vector<uint8_t> *pickLine(
	std::vector<uint8_t> &unparsedRequestRaw
)
{
	size_t newlinePos;
	size_t unparsedRequestRawSize = unparsedRequestRaw.size();
	std::vector<uint8_t> *requestRawLine;
	for (newlinePos = 0; newlinePos < unparsedRequestRawSize; ++newlinePos) {
		if (unparsedRequestRaw[newlinePos] == '\n') {
			break;
		}
	}
	if (newlinePos == unparsedRequestRawSize) {
		return NULL;
	}
	bool hasCarriageReturn = 0 < newlinePos && unparsedRequestRaw[newlinePos - 1] == '\r';
	requestRawLine = new std::vector<uint8_t>(
		unparsedRequestRaw.begin(),
		unparsedRequestRaw.begin() + newlinePos - (hasCarriageReturn ? 1 : 0)
	);
	unparsedRequestRaw.erase(unparsedRequestRaw.begin(), unparsedRequestRaw.begin() + newlinePos + 1);
	return requestRawLine;
}

}	 // namespace utils

}	 // namespace webserv
