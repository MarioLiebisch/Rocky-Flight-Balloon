#pragma once
#include <SFML/Graphics.hpp>
#include "object.h"
#include "hazard.h"

class Solid;

class Rock : public Hazard {
private:
	float mX;
	float mY;
	float mR;
	float mA;
	bool mL;
	sf::VertexArray mVertices;
	unsigned int mTick;
public:
	Rock(const float x, const float y);
	virtual ~Rock(void);
	void update(std::list<std::shared_ptr<Solid> > &solids, std::list<std::shared_ptr<Hazard> > &hazards);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	virtual bool isHurt(const float x, const float y, const float r) const;
};
