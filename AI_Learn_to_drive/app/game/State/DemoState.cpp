#include "DemoState.h"

#include "app/Application.h"

DemoState::DemoState(Track* track, Application& app)
	: TrackAwareState(track), m_App(app)
{
}

DemoState::~DemoState() {

}

void DemoState::HandleInput() {
	if (IsKeyPressed(KEY_T)) {
		m_App.SetState(StateType::Train);
	}
	if (IsKeyPressed(KEY_E)) {
		m_App.SetState(StateType::Edit);
	}
	if (IsKeyPressed(KEY_R)) {
		m_Car.Reset();
	}
	if (IsKeyPressed(KEY_H)) {
		m_Track->ToggleShowCheckpoints();
	}
}

void DemoState::Update(float dt) {
	m_Car.Update(dt, *m_Track);
}

void DemoState::Render() const {
	m_Track->Render();
	m_Car.Render();

	DrawText("E: Edit", 1050, 10, 20, GRAY);
	DrawText("D: Demo", 1050, 40, 20, GRAY);
	DrawText("R: Reset", 1050, 70, 20, GRAY);
	DrawText("H: Toggle checkpoint", 1050, 100, 20, m_Track->ShowingCheckpoints() ? GREEN : GRAY);
}

void DemoState::Reset() {
	m_Car.Reset();
}