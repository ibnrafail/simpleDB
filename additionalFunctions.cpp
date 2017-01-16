#include "additionalFunctions.h"

bool bothAreSpaces(char ch1, char ch2) { return (ch1 == ch2) && (ch1 == ' '); }

void deleteSpaces(string &str)
{
	std::string::iterator end = std::unique(str.begin(), str.end(), bothAreSpaces);
	str.erase(end, str.end());
}