#include "Help.h"

#include <fstream>
#include <iostream>
#include <random>
#include <chrono>

std::vector<float> Softmax(const std::vector<float>& input) {
    size_t n = input.size();
    std::vector<float> output(n);

    float maxVal = *std::max_element(input.begin(), input.end());

    float sum = 0.0f;
    for (auto& i : input) {
        sum += std::exp(i - maxVal);
    }

    for (size_t i = 0; i < n; i++) {
        output[i] = std::exp(input[i] - maxVal) / sum;
    }

    return output;
}

std::vector<float> ReLU(const std::vector<float>& z){
    std::vector<float> out(z.size());
    for (size_t i = 0; i < z.size(); i++) {
        out[i] = ReLU(z[i]);
    }

    return out;
}

float RandomFloat(float a, float b) {
    static std::random_device rd;   // seed
    static std::mt19937 gen(rd());  // Mersenne Twister engine
    std::uniform_real_distribution<float> dist(a, b);
    return dist(gen);
}