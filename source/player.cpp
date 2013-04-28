#include "player.h"
#include "solid.h"
#include "hazard.h"
#include "game.h"
#include <stack>

Player::Player(const float x, const float y) : mX(x), mY(y), mDX(0), mTick(0), mMovement(MOVE_NONE), mAlive(true) {

}

Player::~Player(void) {

}

void Player::update(std::list<std::shared_ptr<Solid> > &solids, std::list<std::shared_ptr<Hazard> > &hazards) {
	mTick++;

	bool move = false;
	mDX *= 0.99f;

	if (mAlive) {
		mY++;
		if (Game::keyPressed(KEY_LEFT)) {
			mDX = std::max(-2.f, mDX - 0.1f);
			move = true;
		}
		else if (Game::keyPressed(KEY_RIGHT)) {
			mDX = std::min(2.f, mDX + 0.1f);
			move = true;
		}

		for (auto &a: hazards) {
			if (a->isHurt(mX, mY, 16)) {
				mAlive = false;
				Game::playSound("./data/plop.ogg");
				break;
			}
		}
	}
	else {
		mY--;
		mDX *= 0.99f;
	}

	if (std::abs(mDX) < 0.1) {
		mMovement = MOVE_NONE;
		mDX = 0;
	}
	else if (mDX < 0)
		mMovement = MOVE_LEFT;
	else if (mDX > 0)
		mMovement = MOVE_RIGHT;
	mX += mDX;

	for (auto &a: solids) {
		while ((mAlive && a->isSolid(mX - 16, mY)) || a->isSolid(mX - 8, mY - 48)) {
			mX++;
			if (mDX < 0 && !move)
				mDX *= -.25;
		}
		while ((mAlive && a->isSolid(mX + 16, mY)) || a->isSolid(mX + 8, mY - 48)) {
			mX--;
			if (mDX > 0 && !move)
				mDX *= -.25;
		}
	}
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	static sf::CircleShape balloon(16, 24);
	static sf::RectangleShape line(sf::Vector2f(1, 48));
	static sf::CircleShape bhand(4, 16);
	static sf::CircleShape ohand(4, 16);
	static sf::CircleShape body(8, 24);
	static sf::CircleShape lleg(4, 16);
	static sf::CircleShape rleg(4, 16);
	static sf::CircleShape eye(2, 8);
	static bool init = false;
	if (!init) {
		balloon.setOrigin(16, 16);
		balloon.setFillColor(sf::Color::Black);
		line.setOrigin(0, 48);
		line.setFillColor(sf::Color::Black);
		bhand.setOrigin(4, 48);
		bhand.setFillColor(sf::Color::Black);
		ohand.setOrigin(-6, 58);
		ohand.setFillColor(sf::Color::Black);
		body.setOrigin(6, 60);
		body.setFillColor(sf::Color::Black);
		lleg.setOrigin(8, 64);
		lleg.setFillColor(sf::Color::Black);
		rleg.setOrigin(-2, 64);
		rleg.setFillColor(sf::Color::Black);
		eye.setOrigin(2, 54);
		eye.setFillColor(sf::Color::White);
		eye.setScale(0.5f, 1);
	}

	float tmp = std::abs((int)((mTick / 2) % 100) - 50) * 0.004f;

	balloon.setScale(0.9f + tmp, 1.1f - tmp);

	tmp -= mDX / 8;

	balloon.setPosition(mX, mY);
	line.setPosition(mX, mY);
	bhand.setPosition(mX, mY);
	ohand.setPosition(mX + (tmp - 0.1f) * 10, mY);
	body.setPosition(mX + (tmp - 0.1f) * 10, mY);
	lleg.setPosition(mX + (tmp - 0.1f) * 20, mY);
	rleg.setPosition(mX + (tmp - 0.1f) * 20, mY);
	target.draw(ohand, states);
	target.draw(body, states);
	target.draw(lleg, states);
	target.draw(rleg, states);
	target.draw(line, states);
	if (mAlive)
		target.draw(balloon, states);
	target.draw(bhand, states);
	eye.setPosition(mX + (tmp - 0.1f) * 20, mY);
	target.draw(eye, states);
	eye.setPosition(mX + (tmp - 0.1f) * 20 + 4, mY);
	target.draw(eye, states);
}
