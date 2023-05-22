#pragma once
class Road
{
	unsigned long long road[960];
public:
	Road(const char* roadFileName);
	int getPieceOfRoad(int i, int j) const;
	void showConsole();
};

