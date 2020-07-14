#include "olcPixelGameEngine.h"
#include <string>
#include <math.h>
#include "sims.h"
#define PI 3.14159265
#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

struct Boid {
	olc::vd2d pos;
	olc::vd2d vel;
	int num_of_neighbors;
};


static const int num_of_boids = 1500;
static struct Boid boids[num_of_boids];

static const int num_of_variables = 6;
static VarController* var_controllers[num_of_variables];

static long double rule_1_coef = 20000;
static long double rule_2_coef = 2000;
static long double rule_3_coef = 80;
static long double pulling_radius = 4;
static long double max_speed = 1;
static long double mouse_power = 20000;

static const double cos_100 = cos(100 * PI / 360);
static const double sin_100 = sin(100 * PI / 360);
static const double color_k = 30 * 255.0 / (num_of_boids * pulling_radius);

static int key_state = 0;
static long double k = 1;
static bool show_info = true;
static long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
static std::string help_text;

BoidsSim::BoidsSim()
{
	sAppName = "Boids Simulator";
}

bool BoidsSim::OnUserCreate() {
	srand(unsigned int(time(NULL)));
	for (int i = 0; i < num_of_boids; i++)
		boids[i] = {
			olc::vd2d(long double(rand() % ScreenWidth()), long double(rand() % ScreenHeight())),
			olc::vd2d(long double(rand() - (RAND_MAX/2)), long double(rand() - (RAND_MAX / 2))).norm() * max_speed * 0.1 * (rand() % 10),
			0
		};
	var_controllers[0] = new VarController(&rule_1_coef,    olc::Key::K1, "Center of mass force");
	var_controllers[1] = new VarController(&rule_2_coef,    olc::Key::K2, "Avoid collision force");
	var_controllers[2] = new VarController(&rule_3_coef,    olc::Key::K3, "Center of velocity force");
	var_controllers[3] = new VarController(&pulling_radius, olc::Key::K4, "Radius of forces");
	var_controllers[4] = new VarController(&max_speed,      olc::Key::K5, "Max speed");
	var_controllers[5] = new VarController(&mouse_power,    olc::Key::K6, "Mouse power");

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

bool BoidsSim::OnUserUpdate(float fElapsedTime) {
	Clear(olc::BLACK);

	// Check input

	if (GetKey(olc::Key::F1).bPressed)
		show_info = not show_info;

	if (GetKey(olc::Key::R).bPressed) {
		for (int i = 0; i < num_of_boids; i++)
			boids[i] = {
				olc::vd2d(long double(rand() % ScreenWidth()), long double(rand() % ScreenHeight())),
				olc::vd2d(long double(rand() - (RAND_MAX / 2)), long double(rand() - (RAND_MAX / 2))).norm() * max_speed * 0.1 * (rand() % 10),
				0
		};
	}

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
		DrawString(20, 20 * (i+2), var_controllers[i]->desc, color, 1);
		std::string value;
		value = std::to_string(*var_controllers[i]->var);
		DrawString(20, 10 + 20 * (i+2), value, color, 1);
	}


	/*
	DrawString(20, 40, "Force diff:", olc::WHITE, 1);
	DrawString(20, 50, std::to_string(force_diff), olc::WHITE, 1);

	DrawString(20, 60, "Gravity const:", olc::WHITE, 1);
	DrawString(20, 70, std::to_string(G), olc::WHITE, 1);
	*/

	// Calc physic

	for (int i = 0; i < num_of_boids; i++) {

		boids[i].num_of_neighbors = 0;

		olc::vd2d rule_1_p = olc::vd2d(long double(0), long double(0));
		olc::vd2d rule_2_p = olc::vd2d(long double(0), long double(0));
		olc::vd2d rule_3_p = olc::vd2d(long double(0), long double(0));
		olc::vd2d random_p = olc::vd2d(long double(rand() - (RAND_MAX / 2)), long double(rand() - (RAND_MAX / 2)));
		olc::vd2d mouse_p =  olc::vd2d(long double(GetMouseX()), long double(GetMouseY())) - boids[i].pos;
		
		if ((GetMouse(0).bHeld or GetMouse(1).bHeld) and mouse_p.mag2() < pulling_radius * pulling_radius * 1000) {
			if (GetMouse(0).bHeld)
				mouse_p /= mouse_power;
			else if (GetMouse(1).bHeld)
				mouse_p *= mouse_power;
		} else 
			mouse_p = olc::vd2d(0, 0);
			
		random_p = random_p.norm() * max_speed * 0.005;

		for (int j = 0; j < num_of_boids; j++) {
			if (i == j) {
				continue;
			}
			if ((boids[j].pos - boids[i].pos).mag2() < pulling_radius * pulling_radius) {
				rule_2_p -= (boids[j].pos - boids[i].pos) / rule_2_coef;
			}
			if ((boids[j].pos - boids[i].pos).mag2() < pulling_radius * pulling_radius*50) {
				rule_1_p += boids[j].pos;
				rule_3_p += boids[j].vel;
				boids[i].num_of_neighbors += 1;
			}
				
				
				
		}

		if (boids[i].num_of_neighbors != 0) {
			rule_1_p /= boids[i].num_of_neighbors;
			rule_1_p = (rule_1_p - boids[i].pos) / rule_1_coef;

			rule_3_p /= boids[i].num_of_neighbors;
			rule_3_p = (rule_3_p - boids[i].vel) / rule_3_coef;
		}
			
		boids[i].vel += rule_1_p + rule_2_p + rule_3_p + random_p - mouse_p;
		long double mag = boids[i].vel.mag();
		if (mag > max_speed)
			boids[i].vel = (boids[i].vel / mag) * max_speed;
		boids[i].pos += (boids[i].vel * fElapsedTime * 100);
	}

	// Border teleport & Draw 

	for (int i = 0; i < num_of_boids; i++) {
		boids[i].pos += boids[i].vel;

		/*if (boids[i].pos.x > ScreenWidth()) {
			boids[i].pos.x -= ScreenWidth();
		}
		else if (boids[i].pos.x < 0) {
			boids[i].pos.x += ScreenWidth();
		}
		else if (boids[i].pos.y > ScreenHeight()) {
			boids[i].pos.y -= ScreenHeight();
		}
		else if (boids[i].pos.y < 0) {
			boids[i].pos.y += ScreenHeight();
		}*/

		boids[i].pos.x -= boids[i].pos.x * int(boids[i].pos.x / ScreenWidth());
		boids[i].pos.y -= boids[i].pos.y * int(boids[i].pos.y / ScreenHeight());

		//olc::Pixel color = olc::Pixel(int(fElapsedTime *10000) % 255, 255 - map(bodies[i].vel.mag2(), -10000, 10000, 0, 255), 255 - map(bodies[i].pos.mag2(), -10000, 10000, 0, 255));
		int k = min(int(boids[i].num_of_neighbors * color_k), 255);
		olc::Pixel color = olc::Pixel(255, 255 - k, 255 - k);
		//DrawCircle(int(bodies[i].pos.x), int(bodies[i].pos.y), int(bodies[i].mass / 5), color);
		//Draw(int(boids[i].pos.x), int(boids[i].pos.y), color);
		
		// I think this is not working properly, but who cares
		/*if (boids[i].pos.x == INFINITY or boids[i].pos.x == -INFINITY or boids[i].pos.y == INFINITY or boids[i].pos.y == -INFINITY) {
			boids[i].pos = olc::vd2d(long double(rand() % ScreenWidth()), long double(rand() % ScreenHeight()));
			color = olc::BLUE;
		}*/

		olc::vd2d norm_v_p = boids[i].vel.norm();
		int x1 = int(boids[i].pos.x + (norm_v_p.x * 6));
		int y1 = int(boids[i].pos.y + (norm_v_p.y * 6));
		int x2 = int(boids[i].pos.x + (cos_100 * norm_v_p.x - sin_100 * norm_v_p.y) * 2);
		int y2 = int(boids[i].pos.y + (sin_100 * norm_v_p.x + cos_100 * norm_v_p.y) * 2);
		int x3 = int(boids[i].pos.x + (cos_100 * norm_v_p.x + sin_100 * norm_v_p.y) * 2);
		int y3 = int(boids[i].pos.y + (cos_100 * norm_v_p.y - sin_100 * norm_v_p.x) * 2);
		DrawTriangle(x1, y1, x2, y2, x3, y3, color);
	}

	//Draw help box

	if (show_info) {
		olc::vi2d border = { 10,10 };
		olc::vi2d size = GetTextSize(help_text) + border;
		olc::vi2d pos = { (ScreenWidth() - size.x) / 2, (ScreenWidth() - size.y) / 2};
		FillRect(pos, size, olc::BLACK);
		DrawRect(pos, size, olc::WHITE);
		DrawString(pos + (border / 2), help_text, olc::WHITE, 1U);
	}

	return true;
};