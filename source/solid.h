#pragma once

class Solid {
public:
	Solid(void) {}
	virtual ~Solid(void) {}
	virtual bool isSolid(const float x, const float y) const = 0;
};
