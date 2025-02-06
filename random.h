#pragma once
#include <random>
#include <vector>

int rollD(int dice, int bonus=0);

int randomRange(int min, int max);

template<typename T> T randomElement(std::vector<T>& arr)
{
	return arr[rollD(arr.size(), -1)];
}

template<typename T> T randomElementPop(std::vector<T>& arr)
{
	int id = rollD(arr.size(), -1);
	T returnValue = arr[id];
	arr.erase(arr.begin() + id);
	return returnValue;
}