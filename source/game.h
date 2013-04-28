#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <map>
#include <list>

namespace sf {
	class Color;
	class Font;
	class RenderWindow;
	class Sound;
	class SoundBuffer;
	class Texture;
}

enum Screen {
	SCREEN_INIT = 0,
	SCREEN_START,
	SCREEN_PLAY,
	SCREEN_LOSE,
	SCREEN_WIN,
};

enum Key {
	KEY_OK    = 0,
	KEY_BACK  = 1,
	KEY_LEFT  = 2,
	KEY_RIGHT = 4,
};

class Game {
private:
	static const unsigned int mMaxSounds = 10;
	static sf::RenderWindow mWindow;
	static std::map<const std::string, std::unique_ptr<sf::Texture> > mTextures;
	static std::map<const std::string, std::unique_ptr<sf::SoundBuffer> > mSoundBuffers;
	static std::vector<std::unique_ptr<sf::Sound> > mSounds;
	static unsigned int mKeyPressed;
	static unsigned int mKeyTapped;
public:
	static int run(void);
	static sf::Texture &getTexture(const char *file);
	static unsigned int playSound(const char *file);
	static void stopSound(unsigned int i);
	static void freeResources(void);
	static unsigned int getRandom(unsigned int max);
	static unsigned int getRandom(unsigned int min, unsigned int max);
	static bool keyPressed(Key key) { return (mKeyPressed & (1 << key)) != 0; }
	static bool keyTapped(Key key) { return (mKeyTapped & (1 << key)) != 0; }
private:
	Game(void) {}
	~Game(void) {}
	static sf::SoundBuffer &getSoundBuffer(const char *file);
};