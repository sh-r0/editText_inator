#pragma once
#include <string>
#include <vector>

struct gapBuffer_t {
	std::string buff = std::string("", 1000);
	size_t gapSize = 1000;
	size_t gapPosition = 0;

	//inline void moveBuff(size_t _pos) {
	//	memmove(buff.data() + gapPosition, buff.data() + _pos, _pos - gapPosition);
	//	//for (size_t i = gapPosition; i < _pos; i++)
	//	//	buff[i] = buff[i + _pos];
	//	return;
	//}

	gapBuffer_t() : buff(std::string()), gapSize(1000), gapPosition(0) {
		buff.resize(1000);
	}

	gapBuffer_t(gapBuffer_t&& _other) {
		buff = std::move(_other.buff);
		gapPosition = _other.gapPosition;
		gapSize = _other.gapSize;
	}

	gapBuffer_t(const gapBuffer_t& _other) {
		buff = _other.buff;
		gapPosition = _other.gapPosition;
		gapSize = _other.gapSize;
	}

	gapBuffer_t& operator=(gapBuffer_t&& _other) {
		buff = std::move(_other.buff);
		gapPosition = _other.gapPosition;
		gapSize = _other.gapSize;

		return *this;
	}

	gapBuffer_t& operator=(const gapBuffer_t& _other) {
		buff = _other.buff;
		gapPosition = _other.gapPosition;
		gapSize = _other.gapSize;

		return *this;
	}

	constexpr const char* gapBegin() const {
		return (buff.data() + gapPosition);
	}

	constexpr const char* gapEnd() const {
		return (buff.data() + gapPosition + gapSize);
	}

	inline void write(char _c) {
		buff[gapPosition] = _c;
		gapPosition++;
		gapSize--;
		return;
	}

	void write(const std::string& _str) {
		const size_t addSize = 1000;
		if (_str.size() > gapSize) {
			size_t oldPos = gapPosition + gapSize;
			size_t count = buff.size() - (gapPosition + gapSize);
			size_t diff = _str.size() - gapSize;
			buff.resize(buff.size() + addSize + diff);
			memmove(buff.data() + oldPos, buff.data() + count, count);
			gapSize += addSize;
		}

		memcpy(buff.data() + gapPosition, _str.data(), _str.size());
		gapPosition += _str.size();
		gapSize -= _str.size();

		return;
	}

	inline void moveGap(size_t _newPos) {
		if (_newPos > gapPosition) {
			memmove((void*)gapBegin(), (void*)gapEnd(), _newPos - gapPosition);
			gapPosition = _newPos;
		}
		else if (_newPos < gapPosition) {
			memmove((void*)(gapEnd() - (gapPosition - _newPos)), buff.data() + _newPos, gapPosition - _newPos);
			gapPosition = _newPos;
		}
		return;
	}

	inline void toText(std::string& _buff) {
		if (_buff.size() < buff.size())
			_buff.resize(buff.size() - gapSize);
		else _buff.clear();
		memcpy(_buff.data(), buff.data(), gapPosition);
		memcpy(_buff.data() + gapPosition, (void*)gapEnd(), buff.size() - gapSize - gapPosition);
		return;
	}
};

struct sGapBuffer_t {
	std::vector<gapBuffer_t> buff_m;
	size_t gapSize = 999;
	size_t gapPosition = 1;

	//inline void moveBuff(size_t _pos) {
	//	memmove(buff.data() + gapPosition, buff.data() + _pos, _pos - gapPosition);
	//	//for (size_t i = gapPosition; i < _pos; i++)
	//	//	buff[i] = buff[i + _pos];
	//	return;
	//}
	sGapBuffer_t() : buff_m(std::vector<gapBuffer_t>()), gapSize(999), gapPosition(1) {
		buff_m.resize(1000);
		for (size_t i = 0; i < 1000; i++)
			buff_m[i] = gapBuffer_t();
	}

	constexpr const gapBuffer_t* gapBegin() const {
		return (buff_m.data() + gapPosition);
	}

	constexpr const gapBuffer_t* gapEnd() const {
		return (buff_m.data() + gapPosition + gapSize);
	}

	inline void moveGap(size_t _newPos) {
		if (_newPos > gapPosition) {
			size_t count = _newPos - gapPosition;
			for (size_t i = 0; i < count; i++) {
				buff_m[gapPosition + i] = std::move(buff_m[gapPosition + gapSize + i]);
			}
			gapPosition = _newPos;
		}
		else if (_newPos < gapPosition) {
			size_t count = gapPosition - _newPos;
			size_t newBegin = gapPosition + gapSize - count;
			//size_t newBegin = _newPos + gapSize;
			for (size_t i = 0; i < count; i++) {
				buff_m[newBegin + i] = std::move(buff_m[_newPos + i]);
			}
			gapPosition = _newPos;
		}
		return;
	}
};