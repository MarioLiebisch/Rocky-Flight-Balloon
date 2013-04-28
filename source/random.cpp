#include <limits>
#include "random.h"

Random::Random(const unsigned int s) : _index(0) {
	seed(s);
}

void Random::seed(const unsigned int s) {
	_index = 0;
	_storage[0] = s;
	for (unsigned int i = 1; i < 624; ++i) {
		_storage[i] = 0xffffffff & (0x6c078965 * (_storage[i - 1] ^ (_storage[i - 1] >> 30)) + i);
	}
}

unsigned int Random::next(const unsigned int max) {
	if (_index == 0)
		generate();
	if (max == 0)
		return 0;
	unsigned int y = _storage[_index];
	y ^= y >> 11;
	y ^= (y << 7) & 0x9d2c5680;
	y ^= (y << 15) & 0xefc60000;
	y ^= y >> 18;
	_index = (_index + 1) % 624;
	return y % max;
}

void Random::generate(void) {
	unsigned int y;
	for (unsigned int i = 0; i < 624; ++i) {
		y = (0x80000000 & _storage[i]) | (0x7fffffff & ((i + 1) % 624));
		_storage[i] = _storage[(i + 397) % 624] ^ (y >> 1);
		if (y % 2 == 1) {
			_storage[i] ^= 0x9908b0df;
		}
	}
}
