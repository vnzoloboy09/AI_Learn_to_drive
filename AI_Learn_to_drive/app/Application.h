#pragma once

#include "game/Car.h"
#include "game/Track.h"
#include "train/Genetic.h"

class Application
{
public:
	Application(int screenWidth = 800, int screenHeight = 600, const char* title = "title");
	~Application();

	void Run();
	void HandleInput();
	void Update(float dt);
	void Render() const;

	bool IsAllDead();

	void Reset();

private:
	const int m_ScreenWidth;
	const int m_ScreenHeight;
	bool m_Running;

	std::vector<Car> m_Cars;
	Genetic m_Ga;
	size_t m_GenerationCount;
	float m_Timer = 0.0f;

	Track m_Track;
};

