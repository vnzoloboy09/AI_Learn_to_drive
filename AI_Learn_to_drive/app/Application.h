#pragma once

#include "game/Car.h"
#include "game/Track.h"

class Application
{
public:
	Application(int screenWidth = 800, int screenHeight = 600, const char* title = "title");
	~Application();

	void Run();
	void Update(float dt);
	void Render() const;

private:
	const int m_ScreenWidth;
	const int m_ScreenHeight;
	bool m_Running;

	std::vector<Car> m_Cars;
	Car user = Car(true);

	Track m_Track;
};

