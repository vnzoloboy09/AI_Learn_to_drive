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
	m_MousePos = GetMousePosition();
	switch (m_Mode)
	{
	case Mode::Train:
		HandelTrainInput();
		break;
	case Mode::Edit:
		HandelEditInput();
		break;
	default:
		break;
	}
	
}

// ========Handle_Input========
void Application::HandelTrainInput() {
	if (IsKeyDown(KEY_S)) {
		m_Ga.SavePopulation("app/train/save", m_GenerationCount, m_Cars);
	}
	if (IsKeyDown(KEY_R)) {
		user.Reset();
	}
	if (IsKeyDown(KEY_E)) {
		m_Mode = Mode::Edit;
	}
}

void Application::HandelEditInput() {
	if (IsKeyDown(KEY_T)) {
		m_Mode = Mode::Train;
		return;
	}
	if (IsKeyDown(KEY_ONE)) {
		m_EditMode = EditMode::TrackEdit;
	}
	if (IsKeyDown(KEY_TWO)) {
		m_EditMode = EditMode::CheckpointEdit;
	}

	switch (m_EditMode) 
	{
	case EditMode::CheckpointEdit:
	{
		m_SelectedCheckpointId = -1;
		for (size_t i = 0; i < m_Track.checkpoints.size(); i++) {
			if (Vector2Distance(m_MousePos, m_Track.checkpoints[i]) <= 60.0f) {
				m_SelectedCheckpointId = (int)i;
				break;
			}
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && m_SelectedCheckpointId == -1) {
			m_Track.checkpoints.push_back(m_MousePos);
			m_SelectedCheckpointId = (int)m_Track.checkpoints.size() - 1;
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && m_SelectedCheckpointId != -1) {
			m_Track.checkpoints[m_SelectedCheckpointId] = m_MousePos;
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && m_SelectedCheckpointId != -1) {
			m_Track.checkpoints.erase(m_Track.checkpoints.begin() + m_SelectedCheckpointId);
			m_SelectedCheckpointId = -1;
		}
		break;
	}
	case EditMode::TrackEdit:
	{
		if (IsKeyDown(KEY_C)) {
			ImageClearBackground(&m_Track.trackImage, BLACK);
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			ImageDrawCircle(&m_Track.trackImage, (int)m_MousePos.x, (int)m_MousePos.y, 50, BLACK);
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			ImageDrawCircle(&m_Track.trackImage, (int)m_MousePos.x, (int)m_MousePos.y, 50, WHITE);
		}
		UpdateTexture(m_Track.trackTexture, m_Track.trackImage.data);
		break;
	}
	default:
		break;
	}	
}

// ==========Update============
void Application::Update(float dt) {
	switch (m_Mode)
	{
	case Mode::Train:
		UpdateTrain(dt);
		break;
	case Mode::Edit:
		break;
	default:
		break;
	}
}

void Application::UpdateTrain(float dt) {
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

void Application::UpdateEdit() {

}

// ==========Render============
void Application::Render() const {
	BeginDrawing();
	ClearBackground(DARKGRAY);

	switch (m_Mode)
	{
	case Mode::Train:
	{
		m_Track.Render();
		RenderCheckpoints();
		RenderCar();
		break;
	}
	case Mode::Edit:
	{
		m_Track.Render();
		RenderCheckpoints();
		break;
	}
	default:
		break;
	}
	RenderUI();

	EndDrawing();
}

void Application::RenderCheckpoints() const {
	for (auto& checkpoint : m_Track.checkpoints) {
		DrawCircle(static_cast<int>(checkpoint.x), static_cast<int>(checkpoint.y), 60.0f, YELLOW);
	}
}

void Application::RenderUI() const {
	switch (m_Mode)
	{
	case Mode::Train:
	{
		DrawText(TextFormat("Gen: %zu", (m_GenerationCount)), 10, 10, 20, DARKGRAY);
		DrawText(TextFormat("Time: %.3f ms", m_Timer), 10, 40, 20, DARKGRAY);
		DrawText(TextFormat("Best: %.3f", m_BestScore), 10, 70, 20, DARKGRAY);

		DrawText("Press E: Edit mode", 500, 10, 20, GRAY);
		break;
	}
	case Mode::Edit:
	{
		DrawText("Press T: Train mode", 500, 10, 20, GRAY);
		break;
	}
	default:
		break;
	}
	
}

void Application::RenderCar() const {
	for (auto& car : m_Cars) {
		car.Render();
	}
	user.Render();
}

// ==========Other==============
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