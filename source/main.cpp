#include <iostream>
#include <fstream>
#include "game.h"

#ifdef _MSC_VER
	#include <direct.h>
	#define chdir(x) _chdir(x)
#else
	#include <unistd.h>
#endif

int main(int argc, const char *argv[])
{
	if (chdir("data")) {
		if (chdir("..") || chdir("data")) {
			std::cerr << "Couldn't find 'data' directory." << std::endl;
			return -2;
		}
	}
	chdir("..");

	try {
		return Game::run();
	}
	catch(const char *exception) {
		std::cerr << "An exception occured:" << exception << std::endl;
		return -1;
	}
}
