#include "Genetic.h"

#include <algorithm>
#include <fstream>
#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());
const float WEIGHT_LIMIT = 4.0f;

Genetic::Genetic(float mutationRate, float mutationStrength, size_t elitismCount)
	: m_MutationRate(mutationRate), m_MutationStrength(mutationStrength), m_ElitismCount(elitismCount)
{ }

float Genetic::Evolve(std::vector<Car>& population, Vector2 spawnPoint, float spawnAngle) {
	float best = 0.0f;
	std::sort(population.begin(), population.end(), 
		[](const Car& A, const Car& B) {
			return A.GetFitness() > B.GetFitness();
		});
	best = population[0].GetFitness();

	std::vector<Car> nextGen;
	nextGen.reserve(population.size());

	for (size_t i = 0; i < m_ElitismCount && i < population.size(); i++) {
		Car& elite = population[i];
		elite.Reset();
		nextGen.push_back(elite);
	}

	while (nextGen.size() < population.size()) {
		Network parentA = TournamentSelection(population);
		Network parentB = TournamentSelection(population);

		Network childBrain = Crossover(parentA, parentB);
		Mutate(childBrain);

		Car childCar(nextGen.size() - 1);
		childCar.SetBrain(childBrain);
		childCar.Reset(spawnPoint, spawnAngle);
		nextGen.push_back(childCar);
	}

	population = nextGen;
	return best;
}

Network Genetic::TournamentSelection(const std::vector<Car>& population) {
	size_t tournamentSize = 5;
	std::uniform_int_distribution<size_t> dist(0, population.size() - 1);
	size_t bestIndex = dist(gen);
	float bestFitness = population[bestIndex].GetFitness();

	for (size_t i = 0; i < tournamentSize; i++) {
		size_t id = dist(gen);
		if (population[id].GetFitness() > bestFitness) {
			bestFitness = population[id].GetFitness();
			bestIndex = id;
		}
	}

	return population[bestIndex].GetBrain();
}

Network Genetic::Crossover(const Network& ParentA, const Network& parentB) {
	std::uniform_real_distribution<float> chance(0.0f, 1.0f);
	Network child = ParentA;

	for (size_t i = 0; i < child.m_Layers.size(); i++) {
		for (size_t j = 0; j < child.m_Layers[i].biases.size(); j++) {
			if (chance(gen) > 0.5f) {
				child.m_Layers[i].biases[j] = parentB.m_Layers[i].biases[j];
			}
		}

		for (size_t j = 0; j < child.m_Layers[i].weights.size(); j++) {
			if (chance(gen) > 0.5f) {
				child.m_Layers[i].weights[j] = parentB.m_Layers[i].weights[j];
			}
		}
	}

	return child;
}

void Genetic::Mutate(Network& net) {
	std::uniform_real_distribution<float> chance(0.0f, 1.0f);
	std::normal_distribution<float> gaussian(0.0f, m_MutationStrength);

	auto& layers = net.m_Layers;

	for (size_t i = 0; i < layers.size(); i++) {
		for (size_t j = 0; j < layers[i].biases.size(); j++) {
			if (chance(gen) < m_MutationRate) {
				layers[i].biases[j] += gaussian(gen);
				layers[i].biases[j] = std::clamp(layers[i].biases[j], -WEIGHT_LIMIT, WEIGHT_LIMIT);
			}
		}

		for (size_t j = 0; j < layers[i].weights.size(); j++) {
			if (chance(gen) < m_MutationRate) {
				layers[i].weights[j] += gaussian(gen);
				layers[i].weights[j] = std::clamp(layers[i].weights[j], -WEIGHT_LIMIT, WEIGHT_LIMIT);
			}
		}
	}
}

void Genetic::SavePopulation(const std::string& filepath, size_t currentGeneration, std::vector<Car>& population) {
	std::ofstream metaFile(filepath + "/meta.txt");
	if (metaFile.is_open()) {
		metaFile << currentGeneration << "\n";
		metaFile << population.size() << "\n";
		metaFile.close();
	}

	std::sort(population.begin(), population.end(),
		[](const Car& A, const Car& B) {
			return A.GetFitness() > B.GetFitness();
		});

	for (size_t i = 0; i < population.size(); ++i) {
		std::string path = filepath + "/car_" + std::to_string(i) + ".txt";
		population[i].GetBrain().Save(path.c_str());
	}
}

bool Genetic::LoadPopulation(const std::string& filepath, size_t& outGeneration, 
	std::vector<Car>& population, Vector2 spawnPos, float spawnAngle) 
{
	std::ifstream metaFile(filepath + "/meta.txt");
	if (!metaFile.is_open()) {
		return false;
	}

	size_t popSize = 0;
	metaFile >> outGeneration;
	metaFile >> popSize;
	metaFile.close();

	population.clear();
	population.reserve(popSize);

	for (int i = 0; i < popSize; i++) {
		std::string path = filepath + "/car_" + std::to_string(i) + ".txt";

		Network loadedBrain(path.c_str());

		Car car(i);
		car.SetBrain(loadedBrain);
		car.Reset(spawnPos, spawnAngle);
		population.push_back(car);
	}

	return true;
}