#include <http/MessageBody.hpp>

namespace webserv
{

bool MessageBody::pushData(
	uint8_t **data,
	size_t size
)
{
	if (size == 0 || this->_IsProcessComplete) {
		delete[] *data;
		*data = NULL;
		return true;
	}

	if (this->_IsChunked) {
		this->_uncompletedChunk.push_back(MessageBodyChunk(*data, size));
		this->_uncompletedChunkSize += size;
		*data = NULL;
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
	} else if (this->_IsEndWithEOF) {
		this->pushBackToBody(data, size);
	} else {
		size_t remaining = this->_ContentLength - this->_Body.size();
		this->pushBackToBody(data, std::min(remaining, size));
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
	// 最後のパケットの一番最初に\nがあった場合、その前のパケットの最後の文字が\rかどうか確認し、もしそうであればそこで行が切れている
	MessageBodyChunkQueue::iterator tailChunk = this->_uncompletedChunk.end() - 1;
	uint8_t *tailChunkData = tailChunk->first;
	size_t tailChunkSize = tailChunk->second;
	if (this->_uncompletedChunk.size() != 1 && tailChunkData[tailChunkSize - 1] == '\r') {
		size_t sizeDataChunkPos = 0;
		// 1バイト余分だけど、すぐdeleteするし気にしない
		uint8_t *sizeDataChunk = new uint8_t[this->_uncompletedChunkSize - tailChunkSize];
		for (
			MessageBodyChunkQueue::iterator it = this->_uncompletedChunk.begin();
			it != tailChunk;
			++it
		) {
			std::memcpy(sizeDataChunk + sizeDataChunkPos, it->first, it->second);
			sizeDataChunkPos += it->second;
			delete[] it->first;
			it->first = NULL;
		}

		std::memmove(tailChunkData, tailChunkData + 1, tailChunkSize);
		this->_uncompletedChunk.clear();
		this->_uncompletedChunk.push_back(MessageBodyChunk(tailChunkData, tailChunkSize));
		this->_uncompletedChunkSize = tailChunkSize;

		this->_currentChunkSize = hexStrToSize(sizeDataChunk, sizeDataChunkPos);
		this->_isChunkSizeDecoded = true;
		delete[] sizeDataChunk;

		return true;
	}

	size_t crlfPos = 0;
	for (; crlfPos < tailChunkSize - 1; ++crlfPos) {
		if (tailChunkData[crlfPos] == '\r' && tailChunkData[crlfPos + 1] == '\n') {
			size_t tailChunkRemaining = tailChunkSize - crlfPos - 2;
			uint8_t *sizeDataChunk = new uint8_t[this->_uncompletedChunkSize - (tailChunkSize + 2)];
			size_t sizeDataChunkPos = 0;
			for (
				MessageBodyChunkQueue::iterator it = this->_uncompletedChunk.begin();
				it != tailChunk;
				++it
			) {
				std::memcpy(sizeDataChunk + sizeDataChunkPos, it->first, it->second);
				sizeDataChunkPos += it->second;
				delete[] it->first;
				it->first = NULL;
			}

			if (tailChunkRemaining != 0) {
				std::memmove(tailChunkData, tailChunkData + crlfPos + 2, tailChunkRemaining);
				this->_uncompletedChunk.erase(this->_uncompletedChunk.begin(), tailChunk);
				this->_uncompletedChunkSize = tailChunkRemaining;
			} else {
				this->_uncompletedChunk.clear();
				delete[] tailChunkData;
				this->_uncompletedChunkSize = 0;
			}

			this->_currentChunkSize = hexStrToSize(sizeDataChunk, crlfPos);
			this->_isChunkSizeDecoded = true;
			delete[] sizeDataChunk;

			return true;
		}
	}
	return false;
}

bool MessageBody::parseChunkData()
{
	if (this->_uncompletedChunkSize < this->_currentChunkSize + 2) {
		return false;
	}

	MessageBodyChunkQueue::iterator tailChunk = this->_uncompletedChunk.end() - 1;
	uint8_t *tailChunkData = tailChunk->first;
	size_t tailChunkSize = tailChunk->second;

	// 「今」のパケットでchunkが終わったわけでは無い場合、前のパケットの末尾にchunk終了の\rがある可能性がある
	bool isChunkRequiredLengthAlreadyReceivedBeforeCurrentChunk = this->_currentChunkSize <= (this->_uncompletedChunkSize - tailChunkSize);
	// 最後のパケットの一番最初に\nがあった場合、その前のパケットの最後の文字が\rかどうか確認し、もしそうであればそこで行が切れている
	if (isChunkRequiredLengthAlreadyReceivedBeforeCurrentChunk) {
		MessageBodyChunkQueue::iterator tailBeforeChunk = tailChunk - 1;
		if (tailBeforeChunk->first[tailBeforeChunk->second - 1] == '\r' && tailChunkData[0] == '\n') {
			uint8_t *body = new uint8_t[this->_currentChunkSize];
			size_t bodyPos = 0;
			for (
				MessageBodyChunkQueue::iterator it = this->_uncompletedChunk.begin();
				it != tailChunk;
				++it
			) {
				std::memcpy(body + bodyPos, it->first, it->second - (it == tailBeforeChunk ? 1 : 0));
				bodyPos += it->second;
				delete[] it->first;
				it->first = NULL;
			}
			this->pushBackToBody(&body, this->_currentChunkSize);
			std::memmove(tailBeforeChunk->first, tailBeforeChunk->first + 1, tailBeforeChunk->second - 1);
			tailChunk->second -= 1;
			this->_uncompletedChunk.erase(this->_uncompletedChunk.begin(), tailBeforeChunk);
			this->_uncompletedChunkSize -= 1;
			this->_isChunkSizeDecoded = false;
			this->_currentChunkSize = 0;
			return true;
		}
	}

	for (size_t i = 0; i < tailChunkSize - 1; ++i) {
		if (tailChunkData[i] == '\r' && tailChunkData[i + 1] == '\n') {
			this->_uncompletedChunkSize = this->_uncompletedChunkSize - 2 - i;
			if (this->_uncompletedChunk.size() == 1) {
				// uncompletedChunkが処理の後に空になるかチェック
				if (this->_uncompletedChunkSize == 0) {
					this->_uncompletedChunk.clear();
				} else {
					tailChunk->first = new uint8_t[this->_uncompletedChunkSize];
					tailChunk->second = this->_uncompletedChunkSize;
					std::memcpy(tailChunk->first, tailChunkData + i + 2, this->_uncompletedChunkSize);
					this->_uncompletedChunk.erase(this->_uncompletedChunk.begin(), tailChunk);
				}
				this->pushBackToBody(&tailChunkData, this->_currentChunkSize);
				return true;
			}

			uint8_t *body = new uint8_t[this->_currentChunkSize];
			size_t bodyPos = 0;
			for (
				MessageBodyChunkQueue::iterator it = this->_uncompletedChunk.begin();
				it != tailChunk;
				++it
			) {
				std::memcpy(body + bodyPos, it->first, it->second);
				bodyPos += it->second;
				delete[] it->first;
				it->first = NULL;
			}
			std::memcpy(body + bodyPos, tailChunkData, i);
			this->pushBackToBody(&body, this->_currentChunkSize);
			if (this->_uncompletedChunkSize == 0) {
				this->_uncompletedChunk.clear();
			} else {
				std::memmove(tailChunkData, tailChunkData + i + 2, tailChunkSize - i - 2);
				tailChunk->second = tailChunkSize - i - 2;
				this->_uncompletedChunk.erase(this->_uncompletedChunk.begin(), tailChunk);
			}
			return true;
		}
	}

	return false;
}

}	 // namespace webserv
