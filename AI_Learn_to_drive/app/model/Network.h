#pragma once

#include "Layer.h"

class Network {
public:
	Network(const std::vector<size_t>& topology);
	Network(const char* 
	);

	void Forward(const std::vector<float>& inptus);
	void Save(const char* filepath) const;

	const std::vector<float>& GetOutput() const { return m_Layers.back().outputs; }
	const std::vector<Layer>& GetLayers() const { return m_Layers; }

	friend class Genetic;

private:
	std::vector<Layer> m_Layers;
};

