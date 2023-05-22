#define _CRT_SECURE_NO_WARNINGS
#include "Thing.h"
#include <stdio.h>

Thing::Thing(const char* thingFileName, int x, int y)
{
	surface = SDL_LoadBMP(thingFileName);
	if (surface == NULL) {
		char msg[300];
		sprintf(msg, "SDL_LoadBMP(eti.bmp) error: % s\n", SDL_GetError());
		throw msg;
	};
	this->x = x;
	this->y = y;
	speed = 0;
	speedH = 0;
	h = 12;
	w = 6;
	counter = 0;
	active = true;
	reverse = true;
}

void Thing::setSpeed(double speed)
{
	this->speed = speed;
}

void Thing::go()
{
	y += speed;
	counter++;
	if (counter == 250) {
		counter = 0;
		if (reverse) speed = -speed;
	}
	x += speedH;
	if (x <= 4 || x >= 60) {
		reverse = false;
		speed = 0;
		speedH = 0;
	}
}
