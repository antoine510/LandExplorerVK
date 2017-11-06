#include "random.h"
#include <random>

std::random_device rd;
std::mt19937 gen(rd());

template <>
int Random<int>::range(int min, int max) {
	std::uniform_int_distribution<int> dis(min, max);
	return dis(gen);
}

template <>
float Random<float>::range(float min, float max) {
	std::uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}

template class Random<int>;
template class Random<float>;
