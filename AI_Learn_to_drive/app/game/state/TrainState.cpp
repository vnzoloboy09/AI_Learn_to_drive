#include "TrainState.h"

#include "app/Application.h"
#include <raygui.h>

TrainState::TrainState(Track* track, Application& app)
	: TrackAwareState(track), m_UserCar(true), m_App(app)
{
	bool loaded = m_Ga.LoadPopulation("app/train/cars", m_GenerationCount, m_Cars, START_POSITION, START_ANGLE);

	if (!loaded) {
		m_GenerationCount = 1;
		for (int i = 0; i < 110; ++i) {
			m_Cars.emplace_back(false);
		}
	}
}

TrainState::~TrainState() {
}

void TrainState::HandleInput() {
	if (IsKeyPressed(KEY_H)) {
		m_Track->ToggleShowCheckpoints();
	}
}

void TrainState::Update(float dt) {
	m_Timer += dt;
	for (auto& car : m_Cars) {
		car.Update(dt, *m_Track);
	}
	m_UserCar.Update(dt, *m_Track);

	if (IsAllDead() || m_Timer > 35.0f) {
		m_GenerationCount++;
		m_BestScore = m_Ga.Evolve(m_Cars, START_POSITION, START_ANGLE);

		m_Timer = 0.0f;
	}

	m_SaveStatusTimer = std::max(m_SaveStatusTimer - dt, 0.0f);
}

void TrainState::Render() {
	Rectangle simArea = m_App.GetSimArea();
	BeginScissorMode(static_cast<int>(simArea.x), static_cast<int>(simArea.y),
		static_cast<int>(simArea.width), static_cast<int>(simArea.height));
	m_Track->Render();
	for (auto& car : m_Cars) {
		car.Render();
	}
	m_UserCar.Render();
	EndScissorMode();

	RenderUI();
}

void TrainState::RenderUI() {
	Rectangle uiArea = m_App.GetUIArea();

	DrawText(TextFormat("Gen: %zu", (m_GenerationCount)), 10, 10, 20, DARKGRAY);
	DrawText(TextFormat("Time: %.3f ms", m_Timer), 10, 40, 20, DARKGRAY);
	DrawText(TextFormat("Best: %.3f", m_BestScore), 10, 70, 20, DARKGRAY);

	GuiPanel(uiArea, "Control Panel");

	if (GuiButton({ uiArea.x + 20, 40, 170, 35 }, "Edit")) {
		m_App.SetState(StateType::Edit);
	}
	if (GuiButton({ uiArea.x + 210, 40, 170, 35 }, "Watch Demo")) {
		m_App.SetState(StateType::Demo);
	}
	if (GuiButton({ uiArea.x + 20, 85, 170, 35 }, "Save population")) {
		m_Ga.SavePopulation("app/train/cars", m_GenerationCount, m_Cars);
		m_SaveStatusTimer = 3.0f;
	}
	if (GuiButton({ uiArea.x + 210, 85, 170, 35 }, "Reset user car")) {
		m_UserCar.Reset();
	}

	if (m_SaveStatusTimer > 0.0f) {
		DrawText("Saved", uiArea.x + 20, 125, 10, BLUE);
	}
}

void TrainState::Reset() {
	for (auto& car : m_Cars) {
		car.Reset();
	}
	m_UserCar.Render();
	m_Timer = 0.0f;
}

bool TrainState::IsAllDead() {
	for (auto& car : m_Cars) {
		if (car.IsAlive()) {
			return false;
		}
	}
	return true;
}