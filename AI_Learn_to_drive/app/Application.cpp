#include "Application.h"

#include <iostream>
#include <raylib.h>
#include <fstream>
#include <raymath.h>

Application::Application(int screenWidth, int screenHeight, const char* title)
	: m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight)
{
	InitWindow(m_ScreenWidth, m_ScreenHeight, title);
	SetTargetFPS(60);

	m_Cars.reserve(10);
	for (size_t i = 0; i < 10; i++) {
		m_Cars.emplace_back(false);
	}

	m_Track.Load("app/game/track.png");

	m_Running = true;
}

Application::~Application() {
	CloseWindow();
}

void Application::Run() {
	while (m_Running) {
		if (WindowShouldClose()) {
			m_Running = false;
		}
		
		float dt = GetFrameTime();
		Update(dt);
		Render();
	}
} 

void Application::Update(float dt) {
	for (auto& car : m_Cars) {
		car.Update(dt, m_Track);
	}
	user.Update(dt, m_Track);
}

void Application::Render() const {
	BeginDrawing();
	ClearBackground(DARKGRAY);

	m_Track.Render();

	for (auto& car : m_Cars) {
		car.Render();
	}
	user.Render();

	EndDrawing();
}
