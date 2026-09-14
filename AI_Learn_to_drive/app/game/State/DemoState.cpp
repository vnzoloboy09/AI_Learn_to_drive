#include "DemoState.h"

DemoState::DemoState(Track* track)
	: TrackAwareState(track)
{
}

DemoState::~DemoState() {

}

void DemoState::HandleInput() {
	//if (IsKeyPressed(KEY_T)) {
	//	SwitchModeTo(Mode::Train);
	//}
	if (IsKeyPressed(KEY_R)) {
		m_Car.Reset();
	}
	//if (IsKeyPressed(KEY_E)) {
	//	SwitchModeTo(Mode::Edit);
	//}
	if (IsKeyPressed(KEY_H)) {
		m_Track->ToggleShowCheckpoints;
	}
}

void DemoState::Update(float dt) {
	m_Car.Update(dt, *m_Track);
}

void DemoState::Render() const {
	m_Track->Render();
	m_Car.Render();
}