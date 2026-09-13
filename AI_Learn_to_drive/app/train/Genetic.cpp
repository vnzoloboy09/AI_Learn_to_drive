#include "Genetic.h"

#include <algorithm>
#include <fstream>
#include <iostream>

Genetic::Genetic(float mutationRate, float mutationStrength, size_t elitismCount)
	: m_MutationRate(mutationRate), m_MutationStrength(mutationStrength), m_ElitismCount(elitismCount)
{ }

void Genetic::Evolve(std::vector<Car>& population, Vector2 spawnPoint, float spawnAngle) {
	std::sort(population.begin(), population.end(), 
		[](Car A, Car B) {
			return A.GetFitness() > B.GetFitness();
		});

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

		Car childCar(false);
		childCar.SetBrain(childBrain);
		childCar.Reset(spawnPoint, spawnAngle);
		nextGen.push_back(childCar);
	}

	population = nextGen;
}

Network Genetic::TournamentSelection(const std::vector<Car>& population) {
	size_t tournamentSize = 5;
	size_t bestIndex = rand() % population.size();
	float bestFitness = population[bestIndex].GetFitness();

	for (size_t i = 0; i < tournamentSize; i++) {
		size_t id = rand() % population.size();
		if (population[id].GetFitness() > bestFitness) {
			bestFitness = population[id].GetFitness();
			bestIndex = id;
		}
	}

	return population[bestIndex].GetBrain();
}

Network Genetic::Crossover(const Network& ParentA, const Network& parentB) {
	Network child = ParentA;

	for (size_t i = 0; i < child.m_Layers.size(); i++) {
		for (size_t j = 0; j < child.m_Layers[i].biases.size(); j++) {
			if ((rand() % 100) < 50) {
				child.m_Layers[i].biases[j] = parentB.m_Layers[i].biases[j];
			}
		}

		for (size_t j = 0; j < child.m_Layers[i].weights.size(); j++) {
			if ((rand() % 100) < 50) {
				child.m_Layers[i].weights[j] = parentB.m_Layers[i].weights[j];
			}
		}
	}

	return child;
}

void Genetic::Mutate(Network& net) {
	auto& layers = net.m_Layers;

	for (size_t i = 0; i < layers.size(); i++) {
		for (size_t j = 0; j < layers[i].biases.size(); j++) {
			float chance = static_cast<float>(rand()) / RAND_MAX;
			if (chance < m_MutationRate) {
				float mutation = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * m_MutationStrength;
				layers[i].biases[j] += mutation;
			}
		}

		for (size_t j = 0; j < layers[i].weights.size(); j++) {
			float chance = static_cast<float>(rand()) / RAND_MAX;
			if (chance < m_MutationRate) {
				float mutation = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * m_MutationStrength;
				layers[i].weights[j] += mutation;
			}
		}
	}
}

void Genetic::SavePopulation(const std::string& filepath, size_t currentGeneration, const std::vector<Car>& population) {
	std::ofstream metaFile(filepath + "/meta.txt");
	if (metaFile.is_open()) {
		metaFile << currentGeneration << "\n";
		metaFile << population.size() << "\n";
		metaFile.close();
		std::cout << "Saved: " << filepath + "/meta.txt" << '\n';
	}

	for (size_t i = 0; i < population.size(); ++i) {
		std::string path = filepath + "/car_" + std::to_string(i) + ".txt";
		population[i].GetBrain().Save(path.c_str());
		std::cout << "Saved: " + path << '\n';
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

	for (size_t i = 0; i < popSize; ++i) {
		std::string path = filepath + "/car_" + std::to_string(i) + ".txt";

		Network loadedBrain(path.c_str());

		Car car(false);
		car.SetBrain(loadedBrain);
		car.Reset(spawnPos, spawnAngle);
		population.push_back(car);
	}

	return true;
}