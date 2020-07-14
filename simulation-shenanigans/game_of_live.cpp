#include "olcPixelGameEngine.h"
#include "sims.h"

static const int screen_size = 800;
static const int pixel_size = 1;
static void printGrid(bool gridOne[screen_size + 1][screen_size + 1]);
static void determineState(bool gridOne[screen_size + 1][screen_size + 1]);
static void clearScreen(void);
static bool gridOne[screen_size + 1][screen_size + 1] = {};

GameOfLiveSim::GameOfLiveSim() {
	sAppName = "Game Of Live Simulator";
}


bool GameOfLiveSim::OnUserCreate() {
	for (int i = 0; i < screen_size * screen_size / 2; i++)
		gridOne[rand() % screen_size][rand() % screen_size] = true;
	return true;
}

bool GameOfLiveSim::OnUserUpdate(float fElapsedTime) {
	determineState(gridOne);
	for (int x = 0; x < ScreenWidth(); x++)
		for (int y = 0; y < ScreenHeight(); y++)
			if (gridOne[x][y]) {
				Draw(x, y, olc::WHITE);
			}
			else {
				Draw(x, y, olc::Pixel(0, 0, 0));
			}
	return true;
}

void compareGrid(bool gridOne[screen_size + 1][screen_size + 1], bool gridTwo[screen_size + 1][screen_size + 1]) {
	for (int a = 0; a < screen_size; a++)
	{
		for (int b = 0; b < screen_size; b++)
		{
			gridTwo[a][b] = gridOne[a][b];
		}
	}
}

void determineState(bool gridOne[screen_size + 1][screen_size + 1]) {
	bool gridTwo[screen_size + 1][screen_size + 1] = {};
	compareGrid(gridOne, gridTwo);

	for (int a = 1; a < screen_size; a++)
	{
		for (int b = 1; b < screen_size; b++)
		{
			int alive = 0;
			for (int c = -1; c < 2; c++)
			{
				for (int d = -1; d < 2; d++)
				{
					if (!(c == 0 && d == 0))
					{
						if (gridTwo[a + c][b + d])
						{
							++alive;
						}
					}
				}
			}
			if (alive < 2)
			{
				gridOne[a][b] = false;
			}
			else if (alive == 3)
			{
				gridOne[a][b] = true;
			}
			else if (alive > 3)
			{
				gridOne[a][b] = false;
			}
		}
	}
}

//int main()
//{
//	Example demo;
//	if (demo.Construct(gridSize, gridSize, pixelSize, pixelSize))
//		demo.Start();
//	return 0;
//}