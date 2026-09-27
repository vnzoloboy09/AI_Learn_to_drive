#include "DemoState.h"

#include "app/Application.h"

#include <raygui.h>

DemoState::DemoState(Track* track, Application& app)
	: TrackAwareState(track), m_App(app), m_Car(0)
{
	m_Car.SetBrain(Network("app/train/cars/car_0.txt"));
}

DemoState::~DemoState() {

}

void DemoState::HandleInput() {
	if (IsKeyPressed(KEY_E)) {
		m_App.SetState(StateType::Edit);
	}
	if (IsKeyPressed(KEY_T)) {
		m_App.SetState(StateType::Train);
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

void DemoState::Render() {
	Rectangle simArea = m_App.GetSimArea();
	BeginScissorMode(static_cast<int>(simArea.x), static_cast<int>(simArea.y),
		static_cast<int>(simArea.width), static_cast<int>(simArea.height));
	m_Track->Render();
	m_Car.Render();
	EndScissorMode();

	RenderUI();
}

void DemoState::RenderUI() {
	Rectangle uiArea = m_App.GetUIArea();

	GuiPanel(uiArea, "Control Panel");

	if (GuiButton({ uiArea.x + 20, 40, 170, 35 }, "Train [T]")) {
		m_App.SetState(StateType::Train);
	}
	if (GuiButton({ uiArea.x + 210, 40, 170, 35 }, "Edit [E]")) {
		m_App.SetState(StateType::Edit);
	}
	if (GuiButton({ uiArea.x + 20, 85, 170, 35 }, "Reset car [R]")) {
		ResetCar();
	}
	if (m_Track->ShowingCheckpoints()) {
		GuiSetState(STATE_PRESSED);
		if (GuiButton({ uiArea.x + 210, 85, 170, 35 }, "Toggle checkpoints [H]")) {
			m_Track->ToggleShowCheckpoints();
		}
		GuiSetState(STATE_NORMAL);
	}
	else {
		if (GuiButton({ uiArea.x + 210, 85, 170, 35 }, "Toggle checkpoints [H]")) {
			m_Track->ToggleShowCheckpoints();
		}
	}
}

void DemoState::Reset() {
	m_Car.SetBrain(Network("app/train/cars/car_0.txt"));
	ResetCar();
}

void DemoState::ResetCar() {
	m_Car.Reset();
}