#include "Application.h"

#include <raylib.h>

Application::Application(int screenWidth, int screenHeight, const char* title)
	: m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight)
{
	InitWindow(m_ScreenWidth, m_ScreenHeight, title);
	SetTargetFPS(60);

	m_Cars.reserve(10);
	for (size_t i = 0; i < 10; i++) {
		m_Cars.emplace_back(false);
	}

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
		Update(1.0f / 60.0f);
		Render();
	}
} 

void Application::Update(float dt) {
	for (auto& car : m_Cars) {
		car.Update();
	}
}

void Application::Render() const {
	BeginDrawing();
	ClearBackground(DARKGRAY);

	for (auto& car : m_Cars) {
		car.Render();
	}

	DrawRectangleLines(50, 50, 700, 500, LIGHTGRAY);

	EndDrawing();
}