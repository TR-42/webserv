#pragma once

#include <classDefUtils.hpp>
#include <cstdlib>
#include <deque>
#include <http/HttpFieldMap.hpp>
#include <http/exception/BadRequest.hpp>
#include <utility>

namespace webserv
{

typedef std::pair<uint8_t *, size_t> MessageBodyChunk;
typedef std::deque<MessageBodyChunk> MessageBodyChunkQueue;

class MessageBody
{
	DECL_VAR_GETTER(bool, IsEndWithEOF)
	DECL_VAR_GETTER(bool, IsChunked)
	DECL_VAR_GETTER(size_t, ContentLength)
	DECL_VAR_GETTER(bool, IsProcessComplete)
	DECL_VAR_REF_GETTER(MessageBodyChunkQueue, Body)

 private:
	size_t _BodySize;
	bool _isChunkSizeDecoded;
	size_t _currentChunkSize;
	size_t _uncompletedChunkSize;
	MessageBodyChunkQueue _uncompletedChunk;

	bool parseChunkSizeLine();
	bool parseChunkData();

	void pushBackToBody(
		uint8_t **data,
		size_t size
	)
	{
		this->_Body.push_back(MessageBodyChunk(*data, size));
		*data = NULL;
		this->_BodySize += size;
	}

 public:
	MessageBody(
	) : _IsEndWithEOF(false),
			_IsChunked(false),
			_ContentLength(0),
			_IsProcessComplete(true),
			_Body(),
			_BodySize(0),
			_isChunkSizeDecoded(false),
			_currentChunkSize(0),
			_uncompletedChunk(),
			_uncompletedChunkSize(0)
	{
	}
	MessageBody(
		bool isEndWithEOF,
		bool isChunked,
		size_t contentLength
	) : _IsEndWithEOF(isEndWithEOF),
			_IsChunked(isChunked),
			_ContentLength(contentLength),
			_IsProcessComplete(!isEndWithEOF && !isChunked && contentLength == 0),
			_Body(),
			_BodySize(0),
			_isChunkSizeDecoded(false),
			_currentChunkSize(0),
			_uncompletedChunk(),
			_uncompletedChunkSize(0)
	{
	}

	MessageBody(
		const MessageBody &from
	) : _IsEndWithEOF(from._IsEndWithEOF),
			_IsChunked(from._IsChunked),
			_ContentLength(from._ContentLength),
			_IsProcessComplete(from._IsProcessComplete),
			_Body(),
			_BodySize(from._BodySize),
			_isChunkSizeDecoded(from._isChunkSizeDecoded),
			_currentChunkSize(from._currentChunkSize),
			_uncompletedChunk(),
			_uncompletedChunkSize(from._uncompletedChunkSize)
	{
		for (
			MessageBodyChunkQueue::const_iterator it = from._Body.begin();
			it != from._Body.end();
			++it
		) {
			uint8_t *chunk = new uint8_t[it->second];
			std::memcpy(chunk, it->first, it->second);
			this->_Body.push_back(MessageBodyChunk(chunk, it->second));
		}

		for (
			MessageBodyChunkQueue::const_iterator it = from._uncompletedChunk.begin();
			it != from._uncompletedChunk.end();
			++it
		) {
			uint8_t *chunk = new uint8_t[it->second];
			std::memcpy(chunk, it->first, it->second);
			this->_uncompletedChunk.push_back(MessageBodyChunk(chunk, it->second));
		}
	}
	MessageBody &operator=(
		const MessageBody &from
	)
	{
		if (this == &from) {
			return *this;
		}

		this->_IsEndWithEOF = from._IsEndWithEOF;
		this->_IsChunked = from._IsChunked;
		this->_ContentLength = from._ContentLength;
		this->_BodySize = from._BodySize;
		this->_IsProcessComplete = from._IsProcessComplete;
		this->_isChunkSizeDecoded = from._isChunkSizeDecoded;
		this->_currentChunkSize = from._currentChunkSize;
		this->_uncompletedChunkSize = from._uncompletedChunkSize;

		for (
			MessageBodyChunkQueue::iterator it = this->_Body.begin();
			it != this->_Body.end();
			++it
		) {
			delete[] it->first;
			it->first = NULL;
		}
		this->_Body.clear();

		for (
			MessageBodyChunkQueue::iterator it = this->_uncompletedChunk.begin();
			it != this->_uncompletedChunk.end();
			++it
		) {
			delete[] it->first;
			it->first = NULL;
		}
		this->_uncompletedChunk.clear();

		for (
			MessageBodyChunkQueue::const_iterator it = from._Body.begin();
			it != from._Body.end();
			++it
		) {
			uint8_t *chunk = new uint8_t[it->second];
			std::memcpy(chunk, it->first, it->second);
			this->_Body.push_back(MessageBodyChunk(chunk, it->second));
		}

		for (
			MessageBodyChunkQueue::const_iterator it = from._uncompletedChunk.begin();
			it != from._uncompletedChunk.end();
			++it
		) {
			uint8_t *chunk = new uint8_t[it->second];
			std::memcpy(chunk, it->first, it->second);
			this->_uncompletedChunk.push_back(MessageBodyChunk(chunk, it->second));
		}

		return *this;
	}
	~MessageBody()
	{
		for (
			MessageBodyChunkQueue::iterator it = this->_Body.begin();
			it != this->_Body.end();
			++it
		) {
			delete[] it->first;
			it->first = NULL;
		}

		for (
			MessageBodyChunkQueue::iterator it = this->_uncompletedChunk.begin();
			it != this->_uncompletedChunk.end();
			++it
		) {
			delete[] it->first;
			it->first = NULL;
		}
	}

	inline size_t size(
	) const
	{
		return this->_BodySize;
	}

	inline bool empty(
	) const
	{
		return this->_Body.empty();
	}

	inline uint8_t *data(
	) const
	{
		uint8_t *data = new uint8_t[this->_Body.size()];

		size_t offset = 0;
		for (
			MessageBodyChunkQueue::const_iterator it = this->_Body.begin();
			it != this->_Body.end();
			++it
		) {
			std::memcpy(data + offset, it->first, it->second);
			offset += it->second;
		}

		return data;
	}

	static inline MessageBody init(
		const HttpFieldMap &fieldMap,
		bool isEndWithEOF
	)
	{
		bool isChunked = false;
		size_t contentLength = 0;

		bool hasContentLength = fieldMap.isNameExists("Content-Length");
		// Transfer-Encodingが存在する場合、chunkedとみなす (chunked以外非対応のため、事前にバリデーション済みのはず)
		bool hasTransferEncoding = fieldMap.isNameExists("Transfer-Encoding");

		if (hasContentLength && hasTransferEncoding) {
			throw http::exception::BadRequest("Both Content-Length and Transfer-Encoding exist");
		}

		if (hasContentLength) {
			if (!fieldMap.tryGetContentLength(contentLength)) {
				throw http::exception::BadRequest("Content-Length is invalid");
			}
		} else if (hasTransferEncoding) {
			isChunked = true;
		}

		return MessageBody(
			isEndWithEOF && !hasContentLength && !hasTransferEncoding,
			isChunked,
			contentLength
		);
	}

	/**
	 * @brief Bodyとして受け取ったデータを解析し、Bodyに追加する
	 * @remark 受け取ったポインタはこのクラス内でdeleteされる
	 *
	 * @param data 受け取ったデータのポインタ
	 * @param size 受け取ったデータのサイズ
	 * @return true 操作に成功した
	 * @return false 操作に失敗した
	 */
	bool pushData(
		uint8_t **data,
		size_t size
	);
};

}	 // namespace webserv
