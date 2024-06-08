#include <http/MessageBody.hpp>

namespace webserv
{

bool MessageBody::pushData(
	const uint8_t *data,
	size_t size
)
{
	if (this->_IsProcessComplete) {
		return true;
	}

	if (this->_IsChunked) {
		this->_uncompletedChunk.insert(this->_uncompletedChunk.end(), data, data + size);
		while (!this->_uncompletedChunk.empty()) {
			if (!this->_isChunkSizeDecoded && !this->parseChunkSizeLine()) {
				break;
			}
			if (this->_currentChunkSize == 0) {
				this->_IsProcessComplete = true;
				break;
			}
			if (!this->parseChunkData()) {
				break;
			}
		}
	} else {
		size_t remaining = this->_ContentLength - this->_Body.size();
		this->_Body.insert(this->_Body.end(), data, data + std::min(remaining, size));
		this->_IsProcessComplete = this->_Body.size() == this->_ContentLength;
	}
	return true;
}

static size_t hexStrToSize(
	const uint8_t *hexStr,
	size_t length
)
{
	if ((sizeof(size_t) * 2) < length) {
		throw http::exception::BadRequest("Invalid hex string (too long)");
	}

	size_t size = 0;
	for (size_t i = 0; i < length; ++i) {
		size <<= 4;
		if ('0' <= hexStr[i] && hexStr[i] <= '9') {
			size += hexStr[i] - '0';
		} else if ('A' <= hexStr[i] && hexStr[i] <= 'F') {
			size += hexStr[i] - 'A' + 10;
		} else if ('a' <= hexStr[i] && hexStr[i] <= 'f') {
			size += hexStr[i] - 'a' + 10;
		} else {
			throw http::exception::BadRequest("Invalid hex string (invalid character)");
		}
	}

	return size;
}

bool MessageBody::parseChunkSizeLine()
{
	size_t crlfPos = 0;
	for (; crlfPos < this->_uncompletedChunk.size() - 1; ++crlfPos) {
		if (this->_uncompletedChunk[crlfPos] == '\r' && this->_uncompletedChunk[crlfPos + 1] == '\n') {
			this->_currentChunkSize = hexStrToSize(this->_uncompletedChunk.data(), crlfPos);
			this->_isChunkSizeDecoded = true;
			this->_uncompletedChunk.erase(this->_uncompletedChunk.begin(), this->_uncompletedChunk.begin() + crlfPos + 2);
			return true;
		}
	}
	return false;
}

bool MessageBody::parseChunkData()
{
	if (this->_uncompletedChunk.size() < this->_currentChunkSize + 2) {
		return false;
	}

	for (size_t i = 0; i < this->_uncompletedChunk.size() - 1; ++i) {
		if (this->_uncompletedChunk[i] == '\r' && this->_uncompletedChunk[i + 1] == '\n') {
			this->_currentChunkSize = i;
			this->_Body.insert(this->_Body.end(), this->_uncompletedChunk.begin(), this->_uncompletedChunk.begin() + this->_currentChunkSize);
			this->_uncompletedChunk.erase(this->_uncompletedChunk.begin(), this->_uncompletedChunk.begin() + i + 2);
			this->_isChunkSizeDecoded = false;
			this->_currentChunkSize = 0;
			return true;
		}
	}

	return false;
}

}	 // namespace webserv
