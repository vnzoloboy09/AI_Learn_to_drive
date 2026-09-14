#include "Application.h"

#include <iostream>
#include <raylib.h>
#include <fstream>
#include <raymath.h>
#include <string>

Application::Application(int screenWidth, int screenHeight, const char* title)
	: m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight),
	m_User(true), m_DemoCar(false)
{
	InitWindow(m_ScreenWidth, m_ScreenHeight, title);
	SetTargetFPS(60);

	bool loaded = m_Ga.LoadPopulation("app/train/cars", m_GenerationCount, m_Cars, START_POSITION, START_ANGLE);

	if (!loaded) {
		m_GenerationCount = 1;
		for (int i = 0; i < 110; ++i) {
			m_Cars.emplace_back(false);
		}
	}

	m_Track.Load("app/train/tracks/track_01"); //default track

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
		HandleTrainInput();
		break;
	case Mode::Edit:
		HandleEditInput();
		break;
	case Mode::Demo:
		HandleDemoInput();
		break;
	default:
		break;
	}
	
}

// ========Handle_Input========
void Application::HandleTrainInput() {
	if (IsKeyPressed(KEY_S)) {
		m_Ga.SavePopulation("app/train/cars", m_GenerationCount, m_Cars);
	}
	if (IsKeyPressed(KEY_E)) {
		SwitchModeTo(Mode::Edit);
	}
	if (IsKeyPressed(KEY_D)) {
		SwitchModeTo(Mode::Demo);
	}
	if (IsKeyPressed(KEY_R)) {
		m_User.Reset();
	}
	if (IsKeyPressed(KEY_H)) {
		m_ShowCheckpoint = !m_ShowCheckpoint;
	}
}

void Application::HandleEditInput() {
	if (IsKeyPressed(KEY_T)) {
		SwitchModeTo(Mode::Train);
	}
	if (IsKeyPressed(KEY_D)) {
		SwitchModeTo(Mode::Demo);
	}
	if (IsKeyPressed(KEY_ONE)) {
		m_EditMode = EditMode::TrackEdit;
	}
	if (IsKeyPressed(KEY_TWO)) {
		m_EditMode = EditMode::CheckpointEdit;
	}
	if (IsKeyPressed(KEY_S)) {
		m_Track.Save("app/train/tracks/track_01");
	}

	switch (m_EditMode) 
	{
	case EditMode::CheckpointEdit:
	{
		m_SelectedCheckpointId = -1;
		for (size_t i = 0; i < m_Track.checkpoints.size(); i++) {
			if (Vector2Distance(m_MousePos, m_Track.checkpoints[i]) <= CHECKPOINT_SIZE) {
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
		if (IsKeyPressed(KEY_C)) {
			ImageClearBackground(&m_Track.trackImage, BLACK);
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			ImageDrawCircle(&m_Track.trackImage, (int)m_MousePos.x, (int)m_MousePos.y, CHECKPOINT_SIZE, BLACK);
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			ImageDrawCircle(&m_Track.trackImage, (int)m_MousePos.x, (int)m_MousePos.y, CHECKPOINT_SIZE, WHITE);
		}
		UpdateTexture(m_Track.trackTexture, m_Track.trackImage.data);
		break;
	}
	default:
		break;
	}	
}

void Application::HandleDemoInput() {
	if (IsKeyPressed(KEY_T)) {
		SwitchModeTo(Mode::Train);
	}
	if (IsKeyPressed(KEY_R)) {
		m_DemoCar.Reset();
	}
	if (IsKeyPressed(KEY_E)) {
		SwitchModeTo(Mode::Edit);
	}
	if (IsKeyPressed(KEY_H)) {
		m_ShowCheckpoint = !m_ShowCheckpoint;
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
		UpdateEdit();
		break;
	case Mode::Demo:
		UpdateDemo(dt);
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
	m_User.Update(dt, m_Track);

	if (IsAllDead() || m_Timer > 35.0f) {
		m_GenerationCount++;
		m_BestScore = m_Ga.Evolve(m_Cars, START_POSITION, START_ANGLE);

		m_Timer = 0.0f;
	}
}

void Application::UpdateEdit() {

}

void Application::UpdateDemo(float dt) {
	m_DemoCar.Update(dt, m_Track);
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
		m_User.Render();
		break;
	}
	case Mode::Edit:
	{
		m_Track.Render();
		RenderCheckpoints();
		break;
	}
	case Mode::Demo:
		m_Track.Render();
		RenderCheckpoints();
		m_DemoCar.Render();
		break;
	default:
		break;
	}
	RenderUI();

	EndDrawing();
}

void Application::RenderCheckpoints() const {
	if (!m_ShowCheckpoint) {
		return;
	}

	for (int i = 0; i < m_Track.checkpoints.size(); i++) {
		DrawCircle(static_cast<int>(m_Track.checkpoints[i].x), 
			static_cast<int>(m_Track.checkpoints[i].y), CHECKPOINT_SIZE, YELLOW);
		std::string id = std::to_string(i);
		DrawText(id.c_str(), static_cast<int>(m_Track.checkpoints[i].x - 5), 
			static_cast<int>(m_Track.checkpoints[i].y) - 5, 20, BLACK);
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

		DrawText("E: Edit", 1050, 10, 20, GRAY);
		DrawText("D: Demo", 1050, 40, 20, GRAY);
		DrawText("H: Toggle checkpoint", 1050, 70, 20, m_ShowCheckpoint? GREEN : GRAY);
		break;
	}
	case Mode::Edit:
	{
		DrawText("T: Train",1050, 10, 20, GRAY);
		DrawText("D: Demo", 1050, 40, 20, GRAY);
		DrawText("S: Save track", 1050, 70, 20, GRAY);
		if (m_EditMode == EditMode::TrackEdit) {
			DrawText("1 : Draw track", 1050, 100, 20, GREEN);
			DrawText("2 : Draw checkpoint", 1050, 100, 20, GRAY);
		}
		else {
			DrawText("1 : Draw track", 1050, 70, 20, GRAY);
			DrawText("2 : Draw checkpoint", 1050, 130, 20, GREEN);
		}
		
		break;
	}
	case Mode::Demo:
		DrawText("E: Edit", 1050, 10, 20, GRAY);
		DrawText("D: Demo", 1050, 40, 20, GRAY);
		DrawText("R: Reset", 1050, 70, 20, GRAY);
		DrawText("H: Toggle checkpoint", 1050, 100, 20, m_ShowCheckpoint ? GREEN : GRAY);
		break;
	default:
		break;
	}
	
}

void Application::RenderCar() const {
	for (auto& car : m_Cars) {
		car.Render();
	}
}

// ==========SwitchMode=========
void Application::SwitchModeTo(Mode mode) {
	m_Mode = mode;
	switch (mode)
	{
	case Mode::Train:
		for (auto& car : m_Cars) {
			car.Reset();
		}
		m_Timer = 0.0f;
		break;
	case Mode::Edit:
		m_ShowCheckpoint = true;
		break;
	case Mode::Demo:
		m_DemoCar.SetBrain(Network("app/train/cars/car_0.txt"));
		m_DemoCar.Reset();
		break;
	default:
		break;
	}
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