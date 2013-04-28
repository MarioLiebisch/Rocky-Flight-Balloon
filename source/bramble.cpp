#include "game.h"
#include "bramble.h"

Bramble::Bramble(const float x, const float y, const unsigned int height) : mX(x), mY(y), mLVertices(sf::TrianglesStrip), mRVertices(sf::TrianglesStrip) {
	float nx1 = x - 160;
	float nx2 = x + 160;
	float ny = y;
	float nw = 0;
	float dx1 = 0;
	float dx2 = 0;
	unsigned int cheight = 0;
	while (cheight < height - 64) {
		dx1 = dx1 * .5f - 20 + Game::getRandom(0, 40);
		dx2 = dx2 * .5f - 20 + Game::getRandom(0, 40);
		nx1 = std::max(64.f, std::min(120.f, nx1 + dx1));
		nx2 = std::min(572.f, std::max(520.f, nx2 + dx2));
		if (nx1 > nx2)
			std::swap(nx1, nx2);
		mLeft.push_back((int)nx1);
		mRight.push_back((int)nx2);
		mLVertices.append(sf::Vertex(sf::Vector2f(0, (float)cheight + y), sf::Color::Black));
		mLVertices.append(sf::Vertex(sf::Vector2f(nx1, (float)cheight + y), sf::Color::Black));
		mRVertices.append(sf::Vertex(sf::Vector2f(nx2, (float)cheight + y), sf::Color::Black));
		mRVertices.append(sf::Vertex(sf::Vector2f(640, (float)cheight + y), sf::Color::Black));
		cheight += 32;
	}

	mLVertices.append(sf::Vertex(sf::Vector2f(0, (float)cheight + y), sf::Color::Black));
	mLVertices.append(sf::Vertex(sf::Vector2f(nx1 / 2, cheight + y), sf::Color::Black));
	mRVertices.append(sf::Vertex(sf::Vector2f(640 - (640 - nx2) / 2, (float)cheight + y), sf::Color::Black));
	mRVertices.append(sf::Vertex(sf::Vector2f(640, (float)cheight + y), sf::Color::Black));
}

Bramble::~Bramble(void) {

}

void Bramble::update(std::list<std::shared_ptr<Solid> > &solids, std::list<std::shared_ptr<Hazard> > &hazards) {

}

void Bramble::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(mLVertices);
	target.draw(mRVertices);
}

bool Bramble::isSolid(const float x, const float y) const {
	int ry = (int)std::floor(y / 32);
	
	if (ry < 0 || ry + 1 >= (int) mLeft.size())
		return false;

	int l1 = mLeft[ry];
	int l2 = mLeft[ry + 1];
	int r1 = mRight[ry];
	int r2 = mRight[ry + 1];

	float p = ((int)(y / .32f) % 100) / 100.f;

	float l = l1 * (1 - p) + l2 * p;
	float r = r1 * (1 - p) + r2 * p;

	return x < l || x > r;
}
