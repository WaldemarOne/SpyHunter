#pragma once
extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}
struct Thing
{
	int counter;
	double x, y, w, h;
	double speed;
	double speedH;
	bool active;
	bool reverse;
	SDL_Surface* surface;
	Thing() {};
	Thing(const char* thingFileName, int x, int y);
	void setSpeed(double speed);
	void go();
	/*void enemies(int quantity);*/
};

