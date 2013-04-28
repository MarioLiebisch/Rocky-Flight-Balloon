#pragma once

class Random {
private:
	unsigned int _storage[624];
	unsigned int _index;
public:
	Random(const unsigned int s = 331u);
	~Random(void) {}
	void seed(const unsigned int s);
	unsigned int next(const unsigned int max);
	inline unsigned int next(const unsigned int min, const unsigned int max) { return min + next(max - min); }
private:
	void generate(void);
};
