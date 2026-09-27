#pragma once

#include <vector>
#include <cmath>
#include <string>
#include <cstdint>

inline float ReLU(float x) {
	return std::max(0.0f, x);
}

inline float ReLUDerivative(float x) {
	return (x > 0 ? 1.0f : 0.0f);
}

float RandomFloat(float a, float b);

std::vector<float> ReLU(const std::vector<float>& z);

std::vector<float> Softmax(const std::vector<float>& input);

std::vector<float> Tanh(const std::vector<float>& z);
