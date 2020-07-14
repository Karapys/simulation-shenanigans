#include <iostream>

std::string format(std::string s, std::string delimiter, std::string value) {
	// Stupid c++ 11 don't have format functions, so i wrote my
	size_t pos = 0;
	std::string token;

	pos = s.find(delimiter);
	token = s.substr(0, pos);
	s.erase(0, pos + delimiter.length());

	return token + value + s;
}