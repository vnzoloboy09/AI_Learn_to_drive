#include "TrainState.h"

#include "app/Application.h"

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
	if (IsKeyPressed(KEY_S)) {
		m_Ga.SavePopulation("app/train/cars", m_GenerationCount, m_Cars);
	}
	if (IsKeyPressed(KEY_E)) {
		m_App.SetState(StateType::Edit);
	}
	if (IsKeyPressed(KEY_D)) {
		m_App.SetState(StateType::Demo);
	}
	if (IsKeyPressed(KEY_R)) {
		m_UserCar.Reset();
	}
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
}

void TrainState::Render() const {
	m_Track->Render();
	for (auto& car : m_Cars) {
		car.Render();
	}
	m_UserCar.Render();

	DrawText(TextFormat("Gen: %zu", (m_GenerationCount)), 10, 10, 20, DARKGRAY);
	DrawText(TextFormat("Time: %.3f ms", m_Timer), 10, 40, 20, DARKGRAY);
	DrawText(TextFormat("Best: %.3f", m_BestScore), 10, 70, 20, DARKGRAY);

	DrawText("E: Edit", 1050, 10, 20, GRAY);
	DrawText("D: Demo", 1050, 40, 20, GRAY);
	DrawText("H: Toggle checkpoint", 1050, 70, 20, m_Track->ShowingCheckpoints() ? GREEN : GRAY);
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