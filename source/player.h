#pragma once
#include <SFML/Graphics.hpp>
#include <list>
#include "object.h"

enum Movement {
	MOVE_NONE = 0,
	MOVE_LEFT,
	MOVE_RIGHT,
};

class Solid;

class Player : public sf::Drawable, public Object {
private:
	float mX;
	float mY;
	float mDX;
	unsigned int mTick;
	Movement mMovement;
	bool mAlive;
public:
	Player(const float x, const float y);
	virtual ~Player();
	void update(std::list<std::shared_ptr<Solid> > &solids, std::list<std::shared_ptr<Hazard> > &hazards);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	float getX(void) const { return mX; }
	float getY(void) const { return mY; }
	bool isAlive(void) const { return mAlive; }
};