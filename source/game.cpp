#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "game.h"
#include "random.h"
#include "player.h"
#include "bramble.h"
#include "rock.h"

#if _MSC_VER
#define snprintf _snprintf
#endif

#ifndef NDEBUG
#define RLSTATS
#endif

std::map<const std::string, std::unique_ptr<sf::SoundBuffer> > Game::mSoundBuffers;
std::map<const std::string, std::unique_ptr<sf::Texture> > Game::mTextures;
std::vector<std::unique_ptr<sf::Sound> > Game::mSounds;

unsigned int Game::mKeyPressed = 0;
unsigned int Game::mKeyTapped = 0;

#ifdef NDEBUG
sf::RenderWindow Game::mWindow(sf::VideoMode::getDesktopMode(), "Rocky Flight Balloon", sf::Style::Close | sf::Style::Fullscreen);
#else
sf::RenderWindow Game::mWindow(sf::VideoMode(640, 480), "Rocky Flight Balloon", sf::Style::Close | sf::Style::Titlebar);
#endif

int Game::run(void) {
	static const sf::Time stepTime(sf::milliseconds(10));
	static const sf::Time second(sf::seconds(1));
	static const sf::Time micro(sf::microseconds(1));
	static const int maxSteps = 5;

#ifdef NDEBUG
	mWindow.setVerticalSyncEnabled(true);
	mWindow.setMouseCursorVisible(false);
#else
	mWindow.setVerticalSyncEnabled(false);
#endif

	Screen screen = SCREEN_INIT;
	std::shared_ptr<Player> player;
	std::shared_ptr<Bramble> bramble(new Bramble(320, 0, 480));

	std::list<std::shared_ptr<Solid> > solids;
	std::list<std::shared_ptr<Hazard> > hazards;

	bool running = true;
	sf::Event event;
	sf::Clock updateClock;
	sf::Clock statClock;
	sf::Time updateTime;
	sf::Time statTime;
	int frames = 0;
	int updates = 0;
	int tupdates = 0;
	int fps = 0;
	int ups = 0;

#ifdef RLSTATS
	char tmp[256];
#endif

	sf::Color sky1(0xcc, 0xcc, 0xff);
	sf::Color sky2(0xff, 0xdd, 0xcc);
	sf::Color mc1(0xff, 0xdd, 0xcc);
	sf::Color mc2(0xdd, 0xbb, 0xaa);

	sf::VertexArray sky(sf::TrianglesFan);
	sky.append(sf::Vertex(sf::Vector2f(0, 0), sky1));
	sky.append(sf::Vertex(sf::Vector2f(640, 0), sky1));
	sky.append(sf::Vertex(sf::Vector2f(640, 480), sky2));
	sky.append(sf::Vertex(sf::Vector2f(0, 480), sky2));

	sf::VertexArray mountains(sf::TrianglesStrip);
	mountains.append(sf::Vertex(sf::Vector2f(0, 480), mc1));
	mountains.append(sf::Vertex(sf::Vector2f(0, 400), mc2));
	mountains.append(sf::Vertex(sf::Vector2f(100, 480), mc1));
	mountains.append(sf::Vertex(sf::Vector2f(100, 360), mc2));
	mountains.append(sf::Vertex(sf::Vector2f(200, 480), mc1));
	mountains.append(sf::Vertex(sf::Vector2f(200, 400), mc2));
	mountains.append(sf::Vertex(sf::Vector2f(240, 480), mc1));
	mountains.append(sf::Vertex(sf::Vector2f(240, 380), mc2));
	mountains.append(sf::Vertex(sf::Vector2f(280, 480), mc1));
	mountains.append(sf::Vertex(sf::Vector2f(280, 400), mc2));
	mountains.append(sf::Vertex(sf::Vector2f(320, 480), mc1));
	mountains.append(sf::Vertex(sf::Vector2f(320, 360), mc2));
	mountains.append(sf::Vertex(sf::Vector2f(360, 480), mc1));
	mountains.append(sf::Vertex(sf::Vector2f(360, 360), mc2));
	mountains.append(sf::Vertex(sf::Vector2f(400, 480), mc1));
	mountains.append(sf::Vertex(sf::Vector2f(400, 400), mc2));
	mountains.append(sf::Vertex(sf::Vector2f(500, 480), mc1));
	mountains.append(sf::Vertex(sf::Vector2f(500, 360), mc2));
	mountains.append(sf::Vertex(sf::Vector2f(600, 480), mc1));
	mountains.append(sf::Vertex(sf::Vector2f(600, 400), mc2));

	sf::View menuView(sf::FloatRect(0, 0, 640, 480));
	sf::View gameView(sf::FloatRect(0, 480, 640, -480));

	unsigned int tick = 0;
	unsigned int rock_tick = 50;
	unsigned int dead_ticks = 0;
	const unsigned int goal = 5120;

	int diffvalue = 25;
	int difficulty = 1;

	float py = 0;

	sf::Font menuFont;
	menuFont.loadFromFile("./data/DejaVuSans.ttf");
	
	sf::Text titleText("Rocky Flight Balloon", menuFont, 50);
	sf::FloatRect tb = titleText.getLocalBounds();
	titleText.setStyle(sf::Text::Bold);
	titleText.setOrigin(tb.width / 2, tb.height / 2);

	sf::Text easyText("Difficulty: Easy", menuFont, 25);
	tb = easyText.getLocalBounds();
	easyText.setOrigin(tb.width / 2, tb.height / 2);

	sf::Text mediumText("Difficulty: Normal", menuFont, 25);
	tb = mediumText.getLocalBounds();
	mediumText.setOrigin(tb.width / 2, tb.height / 2);

	sf::Text hardText("Difficulty: Impossible", menuFont, 25);
	tb = hardText.getLocalBounds();
	hardText.setOrigin(tb.width / 2, tb.height / 2);

	sf::Text howtoText("[Left]/[Right]: Steer your balloon.\n[Space]: Start a new game.\n[Escape]: Quit the game.", menuFont, 25);
	tb = howtoText.getLocalBounds();
	howtoText.setOrigin(tb.width / 2, tb.height / 2);

	sf::Text footerText("Released under GNU GPL v3\nLudum Dare 26 (http://www.ludumdare.com/)", menuFont, 12);
	tb = footerText.getLocalBounds();
	footerText.setStyle(sf::Text::Italic);
	footerText.setOrigin(tb.width / 2, tb.height);

	mWindow.setKeyRepeatEnabled(false);

	while (running && mWindow.isOpen()) {
		while (mWindow.pollEvent(event)) {
			switch(event.type) {
			case sf::Event::Closed:
				mWindow.close();
				running = false;
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code) {
				case sf::Keyboard::Space:
				case sf::Keyboard::Return:
					mKeyPressed |= 1 << KEY_OK;
					mKeyTapped |= 1 << KEY_OK;
					break;
				case sf::Keyboard::BackSpace:
				case sf::Keyboard::Escape:
					mKeyPressed |= 1 << KEY_BACK;
					mKeyTapped |= 1 << KEY_BACK;
					break;
				case sf::Keyboard::Left:
				case sf::Keyboard::A:
					mKeyPressed |= 1 << KEY_LEFT;
					mKeyTapped |= 1 << KEY_LEFT;
					mKeyPressed &= ~(1 << KEY_RIGHT);
					break;
				case sf::Keyboard::Right:
				case sf::Keyboard::D:
					mKeyPressed |= 1 << KEY_RIGHT;
					mKeyTapped |= 1 << KEY_RIGHT;
					mKeyPressed &= ~(1 << KEY_LEFT);
					break;
				}
				break;
			case sf::Event::KeyReleased:
				switch (event.key.code) {
				case sf::Keyboard::Space:
				case sf::Keyboard::Return:
					mKeyPressed &= ~(1 << KEY_OK);
					break;
				case sf::Keyboard::BackSpace:
				case sf::Keyboard::Escape:
					mKeyPressed &= ~(1 << KEY_BACK);
					break;
				case sf::Keyboard::Left:
				case sf::Keyboard::A:
					mKeyPressed &= ~(1 << KEY_LEFT);
					break;
				case sf::Keyboard::Right:
				case sf::Keyboard::D:
					mKeyPressed &= ~(1 << KEY_RIGHT);
					break;
				}
				break;
			case sf::Event::JoystickButtonPressed:
				if (event.joystickButton.button == 0) {
					mKeyPressed |= 1 << KEY_OK;
					mKeyTapped |= 1 << KEY_OK;
				}
				else if (event.joystickButton.button == 1) {
					mKeyPressed |= 1 << KEY_BACK;
					mKeyTapped |= 1 << KEY_BACK;
				}
				break;
			case sf::Event::JoystickButtonReleased:
				if (event.joystickButton.button == 0)
					mKeyPressed &= ~(1 << KEY_OK);
				else if (event.joystickButton.button == 1)
					mKeyPressed &= ~(1 << KEY_BACK);
				break;
			case sf::Event::JoystickMoved:
				if (event.joystickMove.axis != 0)
					break;
				if (event.joystickMove.position < -30) {
					if ((mKeyPressed & (1 << KEY_LEFT)) == 0)
						mKeyTapped |= 1 << KEY_LEFT;
					mKeyPressed |= 1 << KEY_LEFT;
				}
				else if (event.joystickMove.position > 30) {
					if ((mKeyPressed & (1 << KEY_RIGHT)) == 0)
						mKeyTapped |= 1 << KEY_RIGHT;
					mKeyPressed |= 1 << KEY_RIGHT;
				}
				else {
					mKeyPressed &= ~(1 << KEY_LEFT);
					mKeyPressed &= ~(1 << KEY_RIGHT);
				}
				break;
			}
		}

		tupdates = 0;
		for (updateTime += updateClock.restart(); running && updateTime > stepTime; updateTime += /*updateClock.restart()*/ - stepTime) {
			if (tupdates < maxSteps) {
				switch (screen) {
				case SCREEN_INIT:
					screen = SCREEN_START;
					player.reset(new Player(320, 0));
					bramble.reset(new Bramble(320, 0, goal));
					solids.clear();
					solids.push_back(bramble);
					hazards.clear();
					dead_ticks = 0;
					rock_tick = 50;
					tick = 0;
					mKeyPressed = mKeyTapped = 0;

					// preload sounds
					getSoundBuffer("./data/plop.ogg");
					getSoundBuffer("./data/pick.ogg");
					getSoundBuffer("./data/start.ogg");
					break;
				case SCREEN_START:
					if (keyTapped(KEY_OK)) {
						playSound("./data/start.ogg");
						screen = SCREEN_PLAY;
					}
					else if (keyTapped(KEY_BACK)) {
						mWindow.close();
						running = false;
					}
					else if (keyTapped(KEY_LEFT)) {
						difficulty = std::max(0, difficulty - 1);
						playSound("./data/pick.ogg");
						switch (difficulty) {
						case 0:
							diffvalue = 50;
							break;
						case 2:
							diffvalue = 12;
							break;
						default:
							diffvalue = 25;
							break;
						}
					}
					else if (keyTapped(KEY_RIGHT)) {
						difficulty = std::min(2, difficulty + 1);
						playSound("./data/pick.ogg");
						switch (difficulty) {
						case 0:
							diffvalue = 30;
							break;
						case 2:
							diffvalue = 10;
							break;
						default:
							diffvalue = 20;
							break;
						}
					}
					break;
				case SCREEN_WIN:
				case SCREEN_LOSE:
					// no time for fancy screens right now :(
					//if (keyTapped(KEY_OK))
						screen = SCREEN_INIT;
					/*else if (keyTapped(KEY_BACK)) {
						mWindow.close();
						running = false;
					}*/
					break;
				default:
					player->update(solids, hazards);
					bramble->update(solids, hazards);
					for(auto &a: hazards)
						a->update(solids, hazards);

					hazards.erase(std::remove_if(hazards.begin(), hazards.end(), [](const std::shared_ptr<Hazard> &a) { return a->toDelete(); }), hazards.end());

					py = player->getY();

					if (py > goal + 64) {
						playSound("./data/finish.ogg");
						screen = SCREEN_WIN;
					}
					else if (py < goal - 480 && player->isAlive() && (tick++ % rock_tick == 0)) {
						hazards.push_back(std::shared_ptr<Rock>(new Rock((float)getRandom(128, 512), py + 480)));
						rock_tick = getRandom(1, 5) * diffvalue;
					}
					else if ((!player->isAlive() && dead_ticks++ >= 200) || keyTapped(KEY_BACK))
						screen = SCREEN_LOSE;
					break;
				}

				mKeyTapped = 0;
				updates++;
				tupdates++;
			}
			else {
				updateTime = sf::Time::Zero;
			}
		}

		mWindow.clear(sf::Color::White);
		switch (screen) {
		case SCREEN_INIT:
		case SCREEN_START:
			mWindow.setView(menuView);
			mWindow.draw(sky);
			mWindow.draw(mountains);
			gameView.setCenter(320, 240);
			mWindow.setView(gameView);
			mWindow.draw(*bramble);
			mWindow.setView(menuView);

			titleText.setColor(sf::Color::Black);
			titleText.setPosition(322, 122);
			mWindow.draw(titleText);
			titleText.setColor(sf::Color::White);
			titleText.setPosition(320, 120);
			mWindow.draw(titleText);

			{
				sf::Text *diffText = 0;
				switch (difficulty) {
				case 0:
					diffText = &easyText;
					break;
				case 2:
					diffText = &hardText;
					break;
				default:
					diffText = &mediumText;
					break;
				}
				diffText->setColor(sf::Color::Black);
				diffText->setPosition(322, 242);
				mWindow.draw(*diffText);
				diffText->setColor(sf::Color::White);
				diffText->setPosition(320, 240);
				mWindow.draw(*diffText);
			}

			howtoText.setColor(sf::Color::Black);
			howtoText.setPosition(322, 362);
			mWindow.draw(howtoText);
			howtoText.setColor(sf::Color::White);
			howtoText.setPosition(320, 360);
			mWindow.draw(howtoText);

			footerText.setColor(sf::Color::Black);
			footerText.setPosition(321, 461);
			mWindow.draw(footerText);
			footerText.setColor(sf::Color::White);
			footerText.setPosition(320, 460);
			mWindow.draw(footerText);
			break;
		case SCREEN_WIN:
		case SCREEN_LOSE:
			mWindow.setView(menuView);
			mWindow.draw(sky);
			mWindow.draw(mountains);
			gameView.setCenter(320, 240);
			mWindow.setView(gameView);
			mWindow.draw(*bramble);
			break;
		case SCREEN_PLAY:
			mWindow.setView(menuView);
			mWindow.draw(sky);
			menuView.setCenter(320, 240 - player->getY() / 50.f);
			mWindow.setView(menuView);
			mWindow.draw(mountains);
			menuView.setCenter(320, 240);

			if (player->isAlive())
				gameView.setCenter(320, std::min(goal - 240.f, std::max(240.f, player->getY() + 120)));
			mWindow.setView(gameView);

			mWindow.draw(*bramble);
			mWindow.draw(*player);
			for(auto &a: hazards)
				mWindow.draw(*a);
			break;
		default:
			mWindow.clear(sf::Color::Magenta);
			break;
		}
		mWindow.display();
		frames++;

		statTime += statClock.restart();
		if ((int)statTime.asSeconds() > 0) {
			fps = frames;
			ups = updates;
			frames = updates = 0;
			statTime = sf::Time::Zero;
#ifdef RLSTATS
			snprintf(tmp, 255, "Rocky Flight Balloon [FPS: %d, UPS: %d]", fps, ups);
			mWindow.setTitle(tmp);
#endif
		}
		sf::sleep(micro);
	}
	player.reset();
	bramble.reset();
	solids.clear();
	hazards.clear();
	freeResources();
	return 0;
}

sf::Texture &Game::getTexture(const char *file) {
	auto a = mTextures.find(file);
	if (a != mTextures.end())
		return *a->second;
	sf::Texture *tex = new sf::Texture();
	tex->loadFromFile(file);
	mTextures.insert(std::make_pair(file, std::unique_ptr<sf::Texture>(tex)));
	return *tex;
}

unsigned int Game::playSound(const char *file) {
	size_t i;
	for (i = 0; i < mSounds.size(); i++) {
		sf::Sound &snd = *mSounds[i];
		if (snd.getStatus() != sf::Sound::Playing) {
			sf::SoundBuffer &sbf = getSoundBuffer(file);
			if (snd.getBuffer() != &sbf)
				snd.setBuffer(sbf);
			else
				snd.stop();
			snd.play();
			return i;
		}
	}
	if (i < mMaxSounds) {
		sf::Sound *snd2 = new sf::Sound(getSoundBuffer(file));
		snd2->play();
		mSounds.push_back(std::unique_ptr<sf::Sound>(snd2));
		return i;
	}
	return -1;
}

void Game::stopSound(unsigned int i) {
	if (i < mSounds.size())
		mSounds[i]->stop();
}

sf::SoundBuffer &Game::getSoundBuffer(const char *file) {
	auto a = mSoundBuffers.find(file);
	if (a != mSoundBuffers.end())
		return *a->second;
	sf::SoundBuffer *snd = new sf::SoundBuffer();
	snd->loadFromFile(file);
	mSoundBuffers.insert(std::make_pair(file, std::unique_ptr<sf::SoundBuffer>(snd)));
	return *snd;
}

void Game::freeResources(void) {
	mTextures.clear();
	for (auto &a : mSounds)
		a->stop();
	mSounds.clear();
	mSoundBuffers.clear();
}

unsigned int Game::getRandom(unsigned int max) {
	return getRandom(0, max);
}

unsigned int Game::getRandom(unsigned int min, unsigned int max) {
	static Random rnd((const unsigned int)time(0));
	return rnd.next(min, max);
}
