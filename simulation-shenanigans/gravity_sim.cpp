#include "olcPixelGameEngine.h"
#include <string>
#include "sims.h"
#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

struct Body{
	olc::vd2d pos;
	olc::vd2d vel;
	float mass;
	int num_of_neighbors;
};

static const int num_of_bodies = 1000;
static struct Body bodies[num_of_bodies+1];

static const int num_of_variables = 5;
static VarController* var_controllers[num_of_variables];

static long double gravity_border = 1000;
static long double G = 10;
static long double force_diff = 0.7;
static long double max_speed = 2;
static long double mouse_power = 2500;

static int key_state = 0;
static long double k = 1;
static bool show_info = true;
static const float color_k = 255.0 / num_of_bodies;
long double mag;
static std::string help_text;

//long map(long x, long in_min, long in_max, long out_min, long out_max)
//{
//	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
//}


GravitySim::GravitySim() {
	sAppName = "Gravity Simulator";
}


bool GravitySim::OnUserCreate() {
	srand(time(NULL));
	for (int i = 0; i < num_of_bodies - 1; i++)
		i[bodies] = {
			olc::vd2d(rand() % ScreenWidth(), rand() % ScreenHeight()),
			olc::vd2d(0, 0),
			float(rand() % 50 + 5),
			0
	};

	var_controllers[0] = new VarController(&gravity_border, olc::Key::K1, "Gravity flip point");
	var_controllers[1] = new VarController(&G,              olc::Key::K2, "Gravitational constant");
	var_controllers[2] = new VarController(&force_diff,     olc::Key::K3, "Ratio of attraction to repulsion");
	var_controllers[3] = new VarController(&max_speed,      olc::Key::K4, "Maximum speed");
	var_controllers[4] = new VarController(&mouse_power,    olc::Key::K5, "Mouse power");

	help_text = format(
		std::string("Press 1,2..% buttons on keybord to select parameter\n") +
		"Use MOUSE WHEEL or ARROWS to change parameters\n" +
		"Hold SHIFT or CTRL or SHIFT+CTRL to change parameters faster\n" +
		"You can use LMB and RMB to affect simulation\n" +
		"Press R to restart\n\n" +
		"                                Press F1 to hide this window",
		"%", std::to_string(num_of_variables));

	return true;
}


bool GravitySim::OnUserUpdate(float fElapsedTime) {
	Clear(olc::BLACK);

	// Set mouse as body
		
	bodies[num_of_bodies - 1] = {
			olc::vd2d(GetMouseX(), GetMouseY()),
			olc::vd2d(0, 0),
			0.0,
			0
	};

	// Check input

	if (GetKey(olc::Key::F1).bPressed)
		show_info = not show_info;

	if (GetKey(olc::Key::R).bPressed)
		for (int i = 0; i < num_of_bodies - 1; i++)
			i[bodies] = {
				olc::vd2d(rand() % ScreenWidth(), rand() % ScreenHeight()),
				olc::vd2d(0, 0),
				float(rand() % 50 + 5),
				0
		};

	if (GetMouse(0).bHeld)
		bodies[num_of_bodies - 1].mass = mouse_power;
	if (GetMouse(1).bHeld)
		bodies[num_of_bodies - 1].mass = -mouse_power;

	if (GetKey(olc::Key::SHIFT).bHeld and GetKey(olc::Key::CTRL).bHeld)
		k = 100;
	else if (GetKey(olc::Key::SHIFT).bHeld)
		k = 10;
	else if (GetKey(olc::Key::CTRL).bHeld)
		k = 0.1;
	else
		k = 1;

	for (int i = 0; i < num_of_variables; i++) {
		if (GetKey(var_controllers[i]->key).bPressed) {
			key_state = i;
		}
	}

	if (GetMouseWheel() > 0)
		*var_controllers[key_state]->var *= k + 1;
	else if (GetKey(olc::Key::UP).bHeld)
		*var_controllers[key_state]->var += k;
	if (GetMouseWheel() < 0)
		*var_controllers[key_state]->var /= k + 1;
	else if (GetKey(olc::Key::DOWN).bHeld)
		*var_controllers[key_state]->var -= k;

	//Draw UI

	DrawString(20, 20, "To show info press F1", olc::GREY, 1);

	olc::Pixel color;
	for (int i = 0; i < num_of_variables; i++) {
		if (i == key_state)
			color = olc::YELLOW;
		else
			color = olc::GREY;
		DrawString(20, 20 * (i + 2), var_controllers[i]->desc, color, 1);
		std::string value;
		value = std::to_string(*var_controllers[i]->var);
		DrawString(20, 10 + 20 * (i + 2), value, color, 1);
	}

	// Calc physic
	for (int i = 0; i < num_of_bodies - 1; i++) {
		for (int j = i + 1; j < num_of_bodies; j++) {
			olc::vd2d vec_dist = bodies[j].pos - bodies[i].pos;

			double mag2 = vec_dist.mag2();
			double force_module = G / mag2;
			vec_dist = vec_dist.norm();
			if (mag2 > gravity_border) {
				bodies[i].vel += vec_dist * (force_module * bodies[j].mass * fElapsedTime);
				bodies[j].vel += vec_dist * (force_module * bodies[i].mass * -1 * fElapsedTime);
			}
			else if (mag2 > 30) {
				bodies[i].vel += vec_dist * (force_module * bodies[j].mass * force_diff * -1 * fElapsedTime);
				bodies[j].vel += vec_dist * (force_module * bodies[i].mass * force_diff * fElapsedTime);
				bodies[i].num_of_neighbors += 1;
				bodies[j].num_of_neighbors += 1;
			}
			else {
				bodies[i].num_of_neighbors += 1;
				bodies[j].num_of_neighbors += 1;
			}
			
		}
	}
	// Border teleport & Draw 

	for (int i = 0; i < num_of_bodies; i++) {
		bodies[i].vel.x = min(bodies[i].vel.x, max_speed);
		bodies[i].vel.y = min(bodies[i].vel.y, max_speed);
		bodies[i].vel.x = max(bodies[i].vel.x, -max_speed);
		bodies[i].vel.y = max(bodies[i].vel.y, -max_speed);
		bodies[i].pos += bodies[i].vel;
		if (bodies[i].pos.x > ScreenWidth())
			bodies[i].pos.x -= ScreenWidth();
		else if (bodies[i].pos.x < 0)
			bodies[i].pos.x += ScreenWidth();
		else if (bodies[i].pos.y > ScreenHeight())
			bodies[i].pos.y -= ScreenHeight();
		else if (bodies[i].pos.y < 0)
			bodies[i].pos.y += ScreenHeight();
		//olc::Pixel color = olc::Pixel(int(fElapsedTime *10000) % 255, 255 - map(bodies[i].vel.mag2(), -10000, 10000, 0, 255), 255 - map(bodies[i].pos.mag2(), -10000, 10000, 0, 255));
		int k = min(int(bodies[i].num_of_neighbors * color_k), 255);
		bodies[i].num_of_neighbors = 0;
		olc::Pixel color = olc::Pixel(255, 255 - k, 255 - k);
		//DrawCircle(int(bodies[i].pos.x), int(bodies[i].pos.y), int(bodies[i].mass / 5), color);
		Draw(int(bodies[i].pos.x), int(bodies[i].pos.y), color);
	}

	//Draw help box

	if (show_info) {
		olc::vi2d border = { 10,10 };
		olc::vi2d size = GetTextSize(help_text) + border;
		olc::vi2d pos = { (ScreenWidth() - size.x) / 2, (ScreenWidth() - size.y) / 2 };
		FillRect(pos, size, olc::BLACK);
		DrawRect(pos, size, olc::WHITE);
		DrawString(pos + (border / 2), help_text, olc::WHITE, 1U);
	}

	return true;
}

//int main()
//{
//	GravitySim demo;
//	int n;
//	std::cin >> n;
//	if (demo.Construct(400, 400, 2, 2))
//		demo.Start();
//	return 0;
//}