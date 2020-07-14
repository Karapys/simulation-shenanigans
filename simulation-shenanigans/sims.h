#pragma once
#include "olcPixelGameEngine.h"

class BoidsSim : public olc::PixelGameEngine {
public:
	BoidsSim();
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
};

class GravitySim : public olc::PixelGameEngine {
public:
	GravitySim();
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
};

class GameOfLiveSim : public olc::PixelGameEngine {
public:
	GameOfLiveSim();
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
};

class Menu : public olc::PixelGameEngine {
public:
	Menu();
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
};

class Button {
public:
	Button(olc::vi2d _p_corner, olc::vi2d _p_size, std::string _text);
	olc::vi2d p_corner;
	olc::vi2d p_size;
	std::string text;
	bool check_in(int x, int y);
	void draw();
};

class VarController {
public:
	VarController(long double* _var, olc::Key _key, std::string _desc);
	long double* var;   // Variable to change
	olc::Key key;       // Specifices that key you need to press to be able to change variable
	std::string desc;   // Description that will be printed to the screen
};

std::string format(std::string s, std::string delimiter, std::string value);