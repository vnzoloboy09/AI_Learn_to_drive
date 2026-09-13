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

	bool loaded = m_Ga.LoadPopulation("app/train/save", m_GenerationCount, m_Cars, START_POSITION, START_ANGLE);

	if (!loaded) {
		m_GenerationCount = 1;
		for (int i = 0; i < 100; ++i) {
			m_Cars.emplace_back(false);
		}
	}

	m_Track.Load("app/game/assets/track.png");

	m_Running = true;
}

Application::~Application() {
	CloseWindow();
}

void Application::Run() {
	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		HandleInput();
		Update(dt);
		Render();
	}
} 

void Application::HandleInput() {
	if (IsKeyDown(KEY_S)) {
		m_Ga.SavePopulation("app/train/save", m_GenerationCount, m_Cars);
	}
	if (IsKeyDown(KEY_R)) {
		user.Reset();
	}

	Vector2 mousePos = GetMousePosition();
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (CheckCollisionPointRec(mousePos, m_PauseButton)) {
			m_Running = !m_Running;
		}
	}
}

void Application::Update(float dt) {
	if (m_Running) {
		Train(dt);
	}
}

void Application::Render() const {
	BeginDrawing();
	ClearBackground(DARKGRAY);

	m_Track.Render();

	RenderCar();
	RenderText();
	RenderButton();

	EndDrawing();
}

void Application::Train(float dt) {
	m_Timer += dt;
	for (auto& car : m_Cars) {
		car.Update(dt, m_Track);
	}
	user.Update(dt, m_Track);

	if (IsAllDead() || m_Timer > 15.0f) {
		m_BestScore = m_Cars[0].GetFitness();
		std::cout << m_BestScore << '\n';
		m_GenerationCount++;
		m_Ga.Evolve(m_Cars, START_POSITION, START_ANGLE);

		m_Timer = 0.0f;
	}
}

void Application::RenderText() const {
	DrawText(TextFormat("Gen: %zu", (m_GenerationCount)), 10, 40, 20, DARKGRAY);
	DrawText(TextFormat("Time: %.3f ms", m_Timer), 10, 70, 20, DARKGRAY);
	DrawText(TextFormat("Best: %.3f", m_BestScore), 10, 100, 20, DARKGRAY);
}

void Application::RenderCar() const {
	for (auto& car : m_Cars) {
		car.Render();
	}
	user.Render();
}

void Application::RenderButton() const {
	Color btnColor = m_Running ? DARKGRAY : DARKGREEN;
	DrawRectangleRec(m_PauseButton, btnColor);
	DrawRectangleLinesEx(m_PauseButton, 2, WHITE);

	const char* btnText = m_Running ? "Pause Training" : "Resume Training";
	DrawText(btnText, static_cast<int>(m_PauseButton.x) + 15, static_cast<int>(m_PauseButton.y) + 12, 14, WHITE);
}

void Application::Reset() {
	for (auto& car : m_Cars) {
		car.Reset();
	}
}

bool Application::IsAllDead() {
	for (auto& car : m_Cars) {
		if (car.IsAlive()) {
			return false;
		}
	}
	return true;
}