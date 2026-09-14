#pragma once

#include "State.h"

class Application;

class EditState : public TrackAwareState
{
public:
	EditState(Track* track, Application& app);
	~EditState();

	void HandleInput() override;
	void Update(float dt) override;
	void Render() const override;
	void Reset() override;

private:
	enum Mode {
		EditCheckpoint = 0,
		EditTrack
	};

	Application& m_App;

	Mode m_Mode = EditCheckpoint;
	int m_SelectedCheckpointId = -1;
};

