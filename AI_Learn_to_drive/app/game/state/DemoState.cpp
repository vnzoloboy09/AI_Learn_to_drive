#include "DemoState.h"

#include "app/Application.h"

DemoState::DemoState(Track* track, Application& app)
	: TrackAwareState(track), m_App(app)
{
	m_Car.SetBrain(Network("app/train/cars/car_0.txt"));
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
		ResetCar();
	}
	if (IsKeyPressed(KEY_H)) {
		m_Track->ToggleShowCheckpoints();
	}
}

void DemoState::Update(float dt) {
	m_Car.Update(dt, *m_Track);
}

void DemoState::Render() {
	Rectangle simArea = m_App.GetSimArea();
	BeginScissorMode(static_cast<int>(simArea.x), static_cast<int>(simArea.y),
		static_cast<int>(simArea.width), static_cast<int>(simArea.height));
	m_Track->Render();
	m_Car.Render();

	DrawText("E: Edit", 1050, 10, 20, GRAY);
	DrawText("D: Demo", 1050, 40, 20, GRAY);
	DrawText("R: Reset", 1050, 70, 20, GRAY);
	DrawText("H: Toggle checkpoint", 1050, 100, 20, m_Track->ShowingCheckpoints() ? GREEN : GRAY);
	EndScissorMode();

	RenderUI();
}

void DemoState::RenderUI() {

}

void DemoState::Reset() {
	m_Car.SetBrain(Network("app/train/cars/car_0.txt"));
	ResetCar();
}

void DemoState::ResetCar() {
	m_Car.Reset();
}