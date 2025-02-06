#include "customOperations.h"


std::string lowercaseString(std::string str)
{
	for (int i = 0; i < str.length(); i++)
	{
		str[i] = std::tolower(str[i]);
	}
	//cout << "To lowercase: " << str << endl;
	return str;
}

int distanceBetween(int x1, int y1, int x2, int y2)
{
	return floor(sqrt(pow(abs(x1 - x2), 2) + pow(abs(y1 - y2), 2)));
}