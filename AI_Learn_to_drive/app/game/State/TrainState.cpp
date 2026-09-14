#include "TrainState.h"

TrainState::TrainState(Track* track)
	: TrackAwareState(track), m_User(true)
{
}

TrainState::~TrainState() {
}

void TrainState::HandleInput() {
	if (IsKeyPressed(KEY_S)) {
		m_Ga.SavePopulation("app/train/cars", m_GenerationCount, m_Cars);
	}
	//if (IsKeyPressed(KEY_E)) {
	//	SwitchModeTo(Mode::Edit);
	//}
	//if (IsKeyPressed(KEY_D)) {
	//	SwitchModeTo(Mode::Demo);
	//}
	if (IsKeyPressed(KEY_R)) {
		m_User.Reset();
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
	m_User.Update(dt, *m_Track);

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
	m_User.Render();
}

bool TrainState::IsAllDead() {
	for (auto& car : m_Cars) {
		if (car.IsAlive()) {
			return false;
		}
	}
	return true;
}