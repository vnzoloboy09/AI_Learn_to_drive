#include "EditState.h"

#include "app/game/Track.h"

#include <raylib.h>
#include <raymath.h>

EditState::EditState(Track* track, const Application& app)
	: TrackAwareState(track), m_App(app)
{
}

EditState::~EditState() {
	
}

void EditState::HandleInput() {
	//if (IsKeyPressed(KEY_T)) {
	//	SwitchModeTo(Mode::Train);
	//}
	//if (IsKeyPressed(KEY_D)) {
	//	SwitchModeTo(Mode::Demo);
	//}
	if (IsKeyPressed(KEY_ONE)) {
		m_Mode = EditTrack;
	}
	if (IsKeyPressed(KEY_TWO)) {
		m_Mode = EditTrack;
	}
	if (IsKeyPressed(KEY_S)) {
		m_Track->Save("app/train/tracks/track_01");
	}

	Vector2 mousePos = m_App.GetMousePos();
	switch (m_Mode)
	{
	case EditCheckpoint:
	{
		m_SelectedCheckpointId = -1;
		for (size_t i = 0; i < m_Track->checkpoints.size(); i++) {
			if (Vector2Distance(mousePos, m_Track->checkpoints[i]) <= CHECKPOINT_SIZE) {
				m_SelectedCheckpointId = (int)i;
				break;
			}
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && m_SelectedCheckpointId == -1) {
			m_Track->checkpoints.push_back(mousePos);
			m_SelectedCheckpointId = (int)m_Track->checkpoints.size() - 1;
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && m_SelectedCheckpointId != -1) {
			m_Track->checkpoints[m_SelectedCheckpointId] = mousePos;
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && m_SelectedCheckpointId != -1) {
			m_Track->checkpoints.erase(m_Track->checkpoints.begin() + m_SelectedCheckpointId);
			m_SelectedCheckpointId = -1;
		}
		break;
	}
	case EditTrack:
	{
		if (IsKeyPressed(KEY_C)) {
			ImageClearBackground(&m_Track->trackImage, BLACK);
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			ImageDrawCircle(&m_Track->trackImage, static_cast<int>(mousePos.x),
				static_cast<int>(mousePos.y), CHECKPOINT_SIZE, BLACK);
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			ImageDrawCircle(&m_Track->trackImage, static_cast<int>(mousePos.x),
				static_cast<int>(mousePos.y), CHECKPOINT_SIZE, WHITE);
		}
		UpdateTexture(m_Track->trackTexture, m_Track->trackImage.data);
		break;
	}
	default:
		break;
	}
}

void EditState::Update(float dt) {
	
}

void EditState::Render() const {
	m_Track->Render();
}