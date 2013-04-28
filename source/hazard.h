#pragma once
#include <SFML/Graphics.hpp>
#include "object.h"

class Hazard : public sf::Drawable, public Object {
public:
	Hazard(void) {}
	virtual ~Hazard(void) {}
	virtual bool isHurt(const float x, const float y, const float r) const = 0;
};
