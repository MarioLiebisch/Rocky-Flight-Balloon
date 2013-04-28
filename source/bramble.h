#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "solid.h"
#include "object.h"

class Bramble : public sf::Drawable, public Solid, public Object {
private:
	const float mX;
	const float mY;
	std::vector<int> mLeft;
	std::vector<int> mRight;
	sf::VertexArray mLVertices;
	sf::VertexArray mRVertices;
public:
	Bramble(const float x, const float y, const unsigned int height);
	virtual ~Bramble(void);
	void update(std::list<std::shared_ptr<Solid> > &solids, std::list<std::shared_ptr<Hazard> > &hazards);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	bool isSolid(const float x, const float y) const;
};
