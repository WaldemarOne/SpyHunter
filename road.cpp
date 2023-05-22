#define _CRT_SECURE_NO_WARNINGS
#include "road.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>

using namespace std;

int getBit(unsigned long long x, int bit) {
	if (x & (NULL << bit)) return 1;
	return 0;
}

Road::Road(const char* roadFileName)
{
	FILE* f = fopen(roadFileName, "rb");
	if (!f) throw "Cannot find file with road...";
	fread(road, sizeof(unsigned long long), 960, f);
	fclose(f);
}

int Road::getPieceOfRoad(int i, int j) const
{
	while (i < 0) i += 960;
	if (i >= 960) i = i % 960;
	unsigned long long x = road[i];
	return getBit(x, j);
}

void Road::showConsole()
{
	cout.fill('0');

	cout << "                           ";
	for (int j = 0; j < 64; j++) {
		cout << hex << j % 16;
	}
	cout << endl;

	int k = 0;
	for (int i = 959; i >=0; i--) {
		cout << setw(3) << i << "  " << setw(20) << road[i] << "  ";
		for (int j = 0; j < 64; j++) {
			int bit = getBit(road[i], j);
			if ( bit) cout << "@";
			if (!bit) cout << " ";
		}
		cout << endl;
		k++;
		if (k % 48 == 0) {
			cout << "                           ";
			for (int j = 0; j < 64; j++) {
				cout << hex << j % 16;
			}
			cout << endl;
		}
	}
	cout.fill(' ');

}
