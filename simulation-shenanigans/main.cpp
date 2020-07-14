#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "sims.h"

const int num_of_buttons = 3;
static Button* menu_buttons[num_of_buttons];
static int sim_to_launch;


Menu::Menu() {
	sAppName = "Kernie's playground";
}

bool Menu::OnUserCreate() {
	sim_to_launch = -1;
	std::string description[num_of_buttons] = { "Start boids simulator", "Start gravity simulator", "Start game of live" };
	for (int i = 0; i < num_of_buttons; i++) {
		menu_buttons[i] = new Button(
			olc::vi2d(ScreenWidth() / 2 - GetTextSize(description[i]).x, (ScreenHeight() - 100) / 2 + i * 40),
			olc::vi2d(GetTextSize(description[i]).x * 2 + 10, 26),
			description[i]
		);
	}
	//GetTextSize(description[i]);
	//DrawString(10, 10 + i * 10, std::to_string(), olc::WHITE, 1);
	return true;
}

bool Menu::OnUserUpdate(float fElapsedTime) {
	Clear(olc::BLACK);
	for (int i = 0; i < num_of_buttons; i++) {
		if (menu_buttons[i]->check_in(GetMouseX(), GetMouseY())) {
			FillRect(menu_buttons[i]->p_corner - olc::vi2d(5, 5), menu_buttons[i]->p_size, olc::VERY_DARK_YELLOW);
			if (GetMouse(0).bPressed) {
				sim_to_launch = i;
				DrawString(olc::vi2d(100, 100), "Enjoy :)", olc::WHITE, 4);
				Menu::olc_Terminate();
			}
		}
		else {
			FillRect(menu_buttons[i]->p_corner - olc::vi2d(5, 5), menu_buttons[i]->p_size, olc::DARK_YELLOW);
		}
		DrawString(menu_buttons[i]->p_corner, menu_buttons[i]->text, olc::WHITE, 2);
		DrawRect(menu_buttons[i]->p_corner - olc::vi2d(5, 5), menu_buttons[i]->p_size, olc::WHITE);

	}
	return true;
}

int main()
{
	Menu menu;

	auto create_boids_sim = [&]() {
		BoidsSim boids;
		int window_size = 800;
		if (boids.Construct(window_size, window_size, 800 / window_size, 800 / window_size))
			boids.Start();
	};

	auto create_gravity_sim = [&]() {
		GravitySim gravity;
		int window_size = 800;
		if (gravity.Construct(window_size, window_size, 800 / window_size, 800 / window_size))
			gravity.Start();
	};

	auto create_game_of_live_sim = [&]() {
		
		GameOfLiveSim game_of_live;
		int window_size = 800;
		if (game_of_live.Construct(window_size, window_size, 800 / window_size, 800 / window_size))
			game_of_live.Start();
	};

	// Not working :(

	
	if (menu.Construct(400, 400, 2, 2)) {
		menu.Start();
		//Sleep(500);
	}
	if (sim_to_launch == 0)
		create_boids_sim();
	if (sim_to_launch == 1)
		create_gravity_sim();
	if (sim_to_launch == 2)
		create_game_of_live_sim();
	return 0;
}