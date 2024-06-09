#pragma once

#include <classDefUtils.hpp>
#include <http/HttpFieldMap.hpp>
#include <http/exception/BadRequest.hpp>

namespace webserv
{

class MessageBody
{
	DECL_VAR_GETTER(bool, IsEndWithEOF)
	DECL_VAR_GETTER(bool, IsChunked)
	DECL_VAR_GETTER(size_t, ContentLength)
	DECL_VAR_GETTER(bool, IsProcessComplete)
	DECL_VAR_REF_GETTER(std::vector<uint8_t>, Body)

 private:
	bool _isChunkSizeDecoded;
	size_t _currentChunkSize;
	std::vector<uint8_t> _uncompletedChunk;

	bool parseChunkSizeLine();
	bool parseChunkData();

 public:
	MessageBody(
	) : _IsEndWithEOF(false),
			_IsChunked(false),
			_ContentLength(0),
			_IsProcessComplete(true),
			_isChunkSizeDecoded(false),
			_currentChunkSize(0),
			_uncompletedChunk()
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
			_isChunkSizeDecoded(false),
			_currentChunkSize(0),
			_uncompletedChunk()
	{
		_uncompletedChunk.reserve(isChunked ? (256 * 256) : contentLength);
	}

	MessageBody(
		const MessageBody &from
	) : _IsEndWithEOF(from._IsEndWithEOF),
			_IsChunked(from._IsChunked),
			_ContentLength(from._ContentLength),
			_IsProcessComplete(from._IsProcessComplete),
			_Body(from._Body),
			_isChunkSizeDecoded(from._isChunkSizeDecoded),
			_currentChunkSize(from._currentChunkSize)
	{
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
		this->_IsProcessComplete = from._IsProcessComplete;
		this->_Body = from._Body;
		this->_isChunkSizeDecoded = from._isChunkSizeDecoded;
		this->_currentChunkSize = from._currentChunkSize;

		return *this;
	}
	~MessageBody() {}

	inline size_t size(
	) const
	{
		return this->_Body.size();
	}

	inline bool empty(
	) const
	{
		return this->_Body.empty();
	}

	inline const uint8_t *data(
	) const
	{
		return this->_Body.data();
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

	bool pushData(
		const uint8_t *data,
		size_t size
	);
};

}	 // namespace webserv
