#include "rock.h"
#include "game.h"

const float PI = 3.14159265359f;

Rock::Rock(const float x, const float y) : mX(x), mY(y), mR((float)Game::getRandom(16, 49)), mA(0), mVertices(sf::TrianglesFan), mTick(0), mL(Game::getRandom(2) > 0) {
	mVertices.append(sf::Vertex(sf::Vector2f(0, 0), sf::Color::Black));
	mVertices.append(sf::Vertex(sf::Vector2f(0, mR), sf::Color::Black));
	for (float a = PI / 12.f; a < 2 * PI; a += PI / 12.f) {
		float tr = mR * .875f + (float)Game::getRandom((unsigned int)(mR / 4));
		mVertices.append(sf::Vertex(sf::Vector2f(sin(a) * tr, cos(a) * tr), sf::Color::Black));
	}
	mVertices.append(sf::Vertex(sf::Vector2f(0, mR), sf::Color::Black));
}

Rock::~Rock(void) {

}

void Rock::update(std::list<std::shared_ptr<Solid> > &solids, std::list<std::shared_ptr<Hazard> > &hazards) {
	if (mTick++ > 600) {
		mQueryDeletion = true;
	}
	mY--;
	if (mL)
		mA -= 8 / mR;
	else
		mA += 8 / mR;
}

void Rock::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(mX, mY).rotate(mA);
	target.draw(mVertices, states);
}

bool Rock::isHurt(const float x, const float y, const float r) const {
	return (r + mR) * (r + mR) >= (mX - x) * (mX - x) + (mY - y) * (mY - y);
}
