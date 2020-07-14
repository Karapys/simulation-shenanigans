#include "sims.h"

Button::Button(olc::vi2d _p_corner, olc::vi2d _p_size, std::string _text) {
	p_corner = _p_corner;
	p_size = _p_size;
	text = _text;
}

bool Button::check_in(int x, int y) {
	return (p_corner.x <= x and x <= p_corner.x + p_size.x and p_corner.y <= y and y < p_corner.y + p_size.y);
}

VarController::VarController(long double* _var, olc::Key _key, std::string _desc) {
	var = _var;
	key = _key;
	desc = _desc;
}