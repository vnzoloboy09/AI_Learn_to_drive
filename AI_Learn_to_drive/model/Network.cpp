#include "Network.h"
#include "Help.h"

#include <fstream>
#include <iostream>

Network::Network(const std::vector<size_t>& topology) {
	m_Layers.reserve(topology.size() - 1);
	for (size_t i = 1; i < topology.size(); i++) {
		m_Layers.emplace_back(topology[i - 1], topology[i]);
	}
}

Network::Network(const char* filename) {
	std::ifstream in(filename, std::ios::binary);
	if (!in.is_open()) {
		std::cerr << "Cant open file: " << filename << '\n';
		return;
	}

	size_t num_layers;
	in.read(reinterpret_cast<char*>(&num_layers), sizeof(size_t));

	for (size_t i = 0; i < num_layers; i++) {
		size_t inputCnt, outputCnt;
		in.read(reinterpret_cast<char*>(&inputCnt), sizeof(size_t));
		in.read(reinterpret_cast<char*>(&outputCnt), sizeof(size_t));

		m_Layers.emplace_back(inputCnt, outputCnt);

		in.read(reinterpret_cast<char*>(m_Layers.back().weights.data()), inputCnt * outputCnt * sizeof(float));
		in.read(reinterpret_cast<char*>(m_Layers.back().biases.data()), outputCnt * sizeof(float));
	}

	in.close();
}

void Network::Forward(const std::vector<float>& inputs) {
	m_Layers[0].Forward(inputs, false);
	for (size_t i = 1; i < m_Layers.size() - 1; i++) {
		m_Layers[i].Forward(m_Layers[i - 1].outputs, false);
	}

	m_Layers.back().Forward(m_Layers[m_Layers.size() - 2].outputs, true);
}

void Network::Save(const char* filename) const {
	std::ofstream out(filename, std::ios::binary);
	if (!out.is_open()) {
		std::cerr << "Cant open file: " << filename << '\n';
		return;
	}

	size_t num_layers = m_Layers.size();
	out.write(reinterpret_cast<const char*>(&num_layers), sizeof(size_t));

	for (auto& layer : m_Layers) {
		out.write(reinterpret_cast<const char*>(&layer.inputCnt), sizeof(size_t));
		out.write(reinterpret_cast<const char*>(&layer.outputCnt), sizeof(size_t));

		out.write(reinterpret_cast<const char*>(layer.weights.data()), layer.weights.size() * sizeof(float));
		out.write(reinterpret_cast<const char*>(layer.biases.data()), layer.biases.size() * sizeof(float));
	}

	out.close();
	std::cout << "Model saved\n";
}
