#pragma once

#include "State.h"

#include <raylib.h>
#include <string>

class Application;

class EditState : public TrackAwareState
{
public:
	EditState(Track* track, Application& app);
	~EditState();

	void HandleInput() override;
	void Update(float dt) override;
	void Render() override;
	void RenderUI() override;
	void Reset() override;

private:
	void ScanTrackFile();

private:
	enum Mode {
		EditCheckpoint = 0,
		EditTrack
	};

	Application& m_App;

	Mode m_Mode = EditCheckpoint;
	int m_SelectedCheckpointId = -1;

	char m_SaveStatusMessage[128] = "";
	Color m_SaveStatusColor = BLACK;
	float m_SaveStatusTimer = 0.0f;

	char m_ListStatusMessage[128] = "";
	Color m_ListStatusColor = BLACK;
	float m_ListStatusTimer = 0.0f;
	int m_ListActive = -1;
	int m_ListScrollIndex = 0;
	FilePathList m_Files;
	std::string m_FileListString = "";
	std::string m_CurrentLoadedFile = "";

	char m_TextBuffer[128] = "file_name";
	bool m_TextBoxEditing = false;

	Rectangle textBoxBounds;
};

