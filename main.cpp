#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include "road.h"
#include "Thing.h"
#include<math.h>
#include<stdio.h>
#include<iostream>
#include<string.h>


extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

char fileName[] = "D:/save_game.spy";

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define TILE_SIZE		10
#define Color int

// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};

// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};

// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};

// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};

// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++) {
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	}
};

bool IsCarOutOfTheRoad(const Road &road, int x, int y) {
	for (int i = y-5; i < y+5; i++) {
		for (int j = x-1; j < x+1; j++) {
			int pieceOfRoad = road.getPieceOfRoad(i, j);
			if (pieceOfRoad == 1) return true;
		}
	}
	return false;
}

bool IsHit(const Thing& car, const Thing& bullet) {
	double dx = car.x - bullet.x;
	double dy = car.y - bullet.y;
	double dist = sqrt(dx * dx + dy * dy);

	return dist < 3;
}

bool IsCollision(int x, int y, const Thing& car) {
	double dx = car.x - x;
	double dy = car.y - y;
	double dist = sqrt(dx * dx + dy * dy);

	return dist < 6;
}

//main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	try {
		Road road("road1.dat");
		road.showConsole();


		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			printf("SDL_Init error: %s\n", SDL_GetError());
			return 1;
		}

		SDL_Window* window;
		SDL_Renderer* renderer;
		int rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
		if (rc != 0) {
			SDL_Quit();
			printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
			return 1;
		};

		SDL_SetWindowTitle(window, " --- Vlad 2023 --- ");


		SDL_Surface* screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

		SDL_Texture* scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING,
			SCREEN_WIDTH, SCREEN_HEIGHT);

		//cursor visibility
		SDL_ShowCursor(SDL_DISABLE);

		// wczytanie obrazka cs8x8.bmp
		SDL_Surface* charset = SDL_LoadBMP("./cs8x8.bmp");
		if (charset == NULL) {
			printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 1;
		};
		SDL_SetColorKey(charset, true, 0x000000);

		//eti = SDL_LoadBMP("./eti.bmp");
		SDL_Surface* car = SDL_LoadBMP("./blue_car.bmp");
		if (car == NULL) {
			printf("SDL_LoadBMP(blue_car.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(charset);
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 1;
		};

		Thing things[20];
		Thing *bullet = NULL; 
		//Thing things[20];
		//Thing *enemies = 0;
		try {
			Thing tree1("./tree.bmp", 3, 100); things[0] = tree1;
			Thing tree2("./tree.bmp", 58, 200); things[1] = tree2;
			Thing tree3("./tree.bmp", 4, 300); things[2] = tree3;
			Thing tree4("./tree.bmp", 33, 400); things[3] = tree4;
			Thing tree5("./tree.bmp", 3, 500); things[4] = tree5;
			Thing tree6("./tree.bmp", 59, 550); things[5] = tree6;
			Thing tree7("./tree.bmp", 4, 600); things[6] = tree7;
			Thing tree8("./tree.bmp", 4, 650); things[7] = tree8;
			Thing tree9("./tree.bmp",  3, 700); things[8] = tree9;
			Thing tree10("./tree.bmp", 51, 800); things[9] = tree10;

			Thing car1("./red_car.bmp", 20, 50); car1.setSpeed(0.1); things[10] = car1;
			Thing car2("./magenta_car.bmp", 40, 150); car2.setSpeed(0.1); things[11] = car2;
			Thing car3("./green_car.bmp", 20, 250); car3.setSpeed(0.1); things[12] = car3;
			Thing car4("./white_car.bmp", 40, 350); car4.setSpeed(0.1); things[13] = car4;
			Thing car5("./red_car.bmp", 20, 450); car5.setSpeed(0.1); things[14] = car5;
			Thing car6("./yellow_car.bmp", 40, 550); car6.setSpeed(0.1); things[15] = car6;
			Thing car7("./green_car.bmp", 40, 650); car7.setSpeed(0.1); things[16] = car7;
			Thing car8("./white_car.bmp", 48, 750); car8.setSpeed(0.1); things[17] = car8;
			Thing car9("./red_car.bmp", 20, 850); car9.setSpeed(0.1); things[18] = car9;
			Thing car10("./magenta_car.bmp", 40, 950); car10.setSpeed(0.1); things[19] = car10;
		}
		catch (const char* msg) {
			printf(msg);
			SDL_FreeSurface(charset);
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 2;
		}

		SDL_Surface* forest_cell = SDL_LoadBMP("./forest.bmp");
		if (forest_cell == NULL) {
			printf("SDL_LoadBMP(forest.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(charset);
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 1;
		};

		SDL_Surface* road_cell = SDL_LoadBMP("./road.bmp");
		if (road_cell == NULL) {
			printf("SDL_LoadBMP(road.bmp) error: %s\n", SDL_GetError());
			SDL_FreeSurface(charset);
			SDL_FreeSurface(screen);
			SDL_DestroyTexture(scrtex);
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			SDL_Quit();
			return 1;
		};

		int xTiles = SCREEN_WIDTH / TILE_SIZE;
		int yTiles = SCREEN_HEIGHT / TILE_SIZE;

		long long start_time = SDL_GetTicks();

		int car_x = 64 / 2;
		int car_y = 48 / 2 + 48 / 3;
		double carSpeed = 0; //meters per second
		double totalDistance = 0;

		int life = 10;
		int quit = 0;
		long long prev_time = 0;
		bool work = true;
		int tDistance;
		while (!quit) {
			if (work) {
				int current_time = SDL_GetTicks();
				int dt = current_time - prev_time; prev_time = current_time;
				int worldTime = current_time - start_time;  //in milliseconds

				totalDistance += carSpeed * dt / 1000.0;
				tDistance = int(totalDistance) % 960;

				if (tDistance < 24) {
					for (int t = 0; t < 20; t++) {
						things[t].active = true;
					}
				}

				int from = tDistance - 8;
				int to = tDistance + 40;

				//road
				int i = 0;
				for (int row = to; row >= from; row--) {
					for (int j = 0; j <= 64; j++) {
						int pieceOfRoad = road.getPieceOfRoad(row, j);
						if (pieceOfRoad == 1) {
							DrawSurface(screen, forest_cell, j * TILE_SIZE, i * TILE_SIZE);
						}
						else {
							DrawSurface(screen, road_cell, j * TILE_SIZE, i * TILE_SIZE);
						}
					}
					i++;
				}

				//insert things
				i = 0;
				for (int row = to; row >= from; row--) {
					for (int j = 0; j <= 64; j++) {
						for (int t = 0; t < 20; t++) {
							if (round(things[t].y) == row && round(things[t].x) == j && things[t].active) {
								DrawSurface(screen, things[t].surface, j * TILE_SIZE, i * TILE_SIZE);
							}
						}
						if (bullet) {
							if (round(bullet->y) == row && round(bullet->x) == j) {
								DrawSurface(screen, bullet->surface, j * TILE_SIZE, i * TILE_SIZE);
							}
						}
					}
					i++;
				}

				if (bullet) {
					if (bullet->counter == 249) {
						delete bullet;
						bullet = NULL;
					}
					else {
						bullet->go();
						for (int t = 0; t < 20; t++) {
							if (IsHit(things[t], *bullet)) {
								things[t].active = false;
								life++;
								delete bullet;
								bullet = NULL;
								break;
							}
						}
					}
				}

				//Collision
				for (int t = 10; t < 20; t++) {
					if (IsCollision(car_x, tDistance, things[t])) {
						life--;
						things[t].speed = carSpeed/60;
						things[t].speedH = (things[t].x - car_x)/30;
						break;
					}
				}


				for (int t = 0; t < 20; t++) {
					things[t].go();
				}

				DrawSurface(screen, car, car_x * 10, car_y * 10);

				//info text
				Color czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
				Color niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
				DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);


				char text[128];
				sprintf(text, "Time: %.2lfs Distance: %.2lfm Speed: %.2lfm/s (\030-faster, \031-slower) Life: %d",
					worldTime / 1000.0, totalDistance, carSpeed, life);
				DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);

				sprintf(text, "Esc-exit, Space-shoot, (p)ause, (f)inish game (n)ew game, (l)-Load, (s)ave ");
				DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

				SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
				//		SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, scrtex, NULL, NULL);
				SDL_RenderPresent(renderer);

				//Out Of The Road
				if (IsCarOutOfTheRoad(road, car_x, tDistance)) {
					if (carSpeed > 10) life--;

					carSpeed = 0;
				}

			}
			//handling of events (if there were any)
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						quit = 1;
					}
					else if (event.key.keysym.sym == SDLK_UP) {
						if (carSpeed < 50.0) carSpeed += 0.5;
					}
					else if (event.key.keysym.sym == SDLK_DOWN) {
						if (carSpeed > 0.1)
							carSpeed -= 0.5;
						else
							carSpeed = 0.5;
					}
					else if (event.key.keysym.sym == SDLK_RIGHT) {
						if (car_x < 61) car_x += 2;
					}
					else if (event.key.keysym.sym == SDLK_LEFT) {
						if (car_x > 3)  car_x -= 2;
					}
					else if (event.key.keysym.sym == SDLK_p) {
						prev_time = SDL_GetTicks();
						work = !work;
					}
					else if (event.key.keysym.sym == SDLK_f) {
						work = false;
					}
					else if (event.key.keysym.sym == SDLK_n) {
						start_time = SDL_GetTicks();

						car_x = 64 / 2;
						car_y = 48 / 2 + 48 / 3;
						carSpeed = 0; //meters per second
						totalDistance = 0;

						life = 5;
						quit = 0;
						prev_time = 0;
						work = true;
					}
					else if (event.key.keysym.sym == SDLK_SPACE) {
						//std::cout << " car_x: " << car_x << "  dist: " << tDistance << std::endl;
						if (bullet) delete bullet;
						bullet = new Thing("bullet.bmp", car_x, tDistance+7);
						bullet->speed =  0.45;
					}
					else if (event.key.keysym.sym == SDLK_s) {
						FILE* f = fopen(fileName, "wb");
						fwrite(&totalDistance, sizeof(totalDistance), 1, f);
						fwrite(&car_x, sizeof(car_x), 1, f);
						fwrite(&carSpeed, sizeof(carSpeed), 1, f);
						fwrite(&life, sizeof(life), 1, f);
						fclose(f);
					}
					else if (event.key.keysym.sym == SDLK_l) {
						work = false;
						prev_time = 0;
						FILE* f = fopen(fileName, "rb");
						fread(&totalDistance, sizeof(totalDistance), 1, f);
						fread(&car_x, sizeof(car_x), 1, f);
						fread(&carSpeed, sizeof(carSpeed), 1, f);
						fread(&life, sizeof(life), 1, f);
						fclose(f);
						work = true;
					}
					break;
				case SDL_KEYUP:
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
		};
		
		//freeing all surfaces
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		SDL_Quit();
		return 0;
	}
	catch (const char* msg) {
		puts(msg);
		return 1;
	}

};


//Color czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
//Color zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
//Color czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
//Color niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

//// console window is not visible, to see the printf output
//// the option:
//// project -> szablon2 properties -> Linker -> System -> Subsystem
//// must be changed to "Console"
//printf("wyjscie printfa trafia do tego okienka\n");
//printf("printf output goes here\n");

//	fullscreen mode
//	int rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);


	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	//SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);



		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplyna³ od ostatniego narysowania ekranu
		// delta to ten sam czas w sekundach
		// here t2-t1 is the time in milliseconds since
		// the last screen was drawn
		// delta is the same time in seconds
//delta = current_time - start_time;
//start_time = current_time;