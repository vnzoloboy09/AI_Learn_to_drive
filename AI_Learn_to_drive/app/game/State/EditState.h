#pragma once

#include "State.h"
#include "app/Application.h"

class EditState : public TrackAwareState
{
public:
	EditState(Track* track, const Application& app);
	~EditState();

	void HandleInput();
	void Update(float dt);
	void Render() const;

private:
	enum Mode {
		EditCheckpoint = 0,
		EditTrack
	};

	const Application& m_App;

	Mode m_Mode = EditCheckpoint;
	int m_SelectedCheckpointId = -1;
};

