#include "random.h"

int rollD(int dice, int bonus)
{
	int result = rand() % dice + 1 + bonus;
	//cout << "d" << dice << " = " << result << endl;
	return result;
}

int randomRange(int min, int max)
{
	if (min > max)
	{
		int temp = max;
		min = max;
		max = temp;
	}
	int result = min + (rand() % (max - min + 1));
	//cout << "d" << dice << " = " << result << endl;
	return result;
}



