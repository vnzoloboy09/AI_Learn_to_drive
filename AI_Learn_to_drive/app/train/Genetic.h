#pragma once

#include "app/model/Network.h"
#include "app/game/Car.h"

#include <string>

class Genetic
{
public:
	Genetic(float mutationRate = 0.15f, float mutationStrength = 0.3f, size_t elitismCount = 5);

	float Evolve(std::vector<Car>& population, Vector2 spawnPoint, float spawnAngle);

	void SavePopulation(const std::string& filepath, size_t currentGeneration, std::vector<Car>& population);
	bool LoadPopulation(const std::string& filepath, size_t& outGeneration, 
		std::vector<Car>& population, Vector2 spawnPos, float spawnAngle);

private:
	Network TournamentSelection(const std::vector<Car>& population);
	Network Crossover(const Network& parentA, const Network& parentB);
	void Mutate(Network& net);

private:
	float m_MutationRate;
	float m_MutationStrength;
	size_t m_ElitismCount;

};

