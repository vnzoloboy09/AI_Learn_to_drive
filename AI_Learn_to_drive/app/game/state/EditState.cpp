#include "EditState.h"

#include "app/game/Track.h"
#include "app/Application.h"

#include <raylib.h>
#include <raymath.h>
#include <filesystem>
#include <raygui.h>

EditState::EditState(Track* track, Application& app)
	: TrackAwareState(track), m_App(app)
{
	ScanTrackFile();
}

EditState::~EditState() {
	UnloadDirectoryFiles(m_Files);
}

void EditState::HandleInput() {
	Vector2 mousePos = m_App.GetMousePos();
	Rectangle simArea = m_App.GetSimArea();;

	textBoxBounds = { m_App.GetUIArea().x + 20, 130, 360, 30 };
	if (GuiTextBox(textBoxBounds, m_TextBuffer, 128, m_TextBoxEditing)) {
		m_TextBoxEditing = !m_TextBoxEditing;
	}

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (CheckCollisionPointRec(mousePos, textBoxBounds)) {
			m_TextBoxEditing = true;
			if (TextIsEqual(m_TextBuffer, "file_name")) {
				m_TextBuffer[0] = '\0';
			}
		}
		else {
			m_TextBoxEditing = false;
			if (m_TextBuffer[0] == '\0') {
				TextCopy(m_TextBuffer, "file_name");
			}
		}
	}

	if (mousePos.x > simArea.width) {
		return;
	}
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
	m_SaveStatusTimer = std::max(m_SaveStatusTimer - dt, 0.0f);
	m_ListStatusTimer = std::max(m_ListStatusTimer - dt, 0.0f);
}

void EditState::Render() {
	Rectangle simArea = m_App.GetSimArea();
	BeginScissorMode(static_cast<int>(simArea.x), static_cast<int>(simArea.y),
		static_cast<int>(simArea.width), static_cast<int>(simArea.height));
	m_Track->Render();
	EndScissorMode();

	RenderUI();
}

void EditState::RenderUI() {
	Rectangle uiArea = m_App.GetUIArea();
	GuiPanel(uiArea, "Control Panel");

	// ================== Change State =========================

	if (GuiButton({ uiArea.x + 20, 40, 170, 35 }, "Train")) {
		m_App.SetState(StateType::Train);
	}
	if (GuiButton({ uiArea.x + 210, 40, 170, 35 }, "Watch Demo")) {
		m_App.SetState(StateType::Demo);
	}

	// ====================== Save ===============================

	DrawText("File Name:", uiArea.x + 20, 105, 10, DARKGRAY);
	if (GuiTextBox(textBoxBounds, m_TextBuffer, 128, m_TextBoxEditing)) {
		m_TextBoxEditing = !m_TextBoxEditing;
	}

	if (GuiButton({ uiArea.x + 20, 175, 360, 30 }, "Save File")) {
		if (m_TextBuffer[0] == '\0' || TextIsEqual(m_TextBuffer, "file_name")) {
			TextCopy(m_SaveStatusMessage, "Error: File name cannot be empty!");
			m_SaveStatusColor = RED;
		}
		else {
			m_Track->Save(m_TextBuffer);
			ScanTrackFile();

			TextCopy(m_SaveStatusMessage, TextFormat("Saved: %s", m_TextBuffer));
			m_SaveStatusColor = DARKGREEN;
		}
		m_SaveStatusTimer = 3.0f;
	}

	if (m_SaveStatusTimer > 0.0f) {
		DrawText(m_SaveStatusMessage, uiArea.x + 20, 205, 10, m_SaveStatusColor);
	}

	// =============== Mode Button ======================
	GuiSetState(STATE_PRESSED);
	if (m_Mode == Mode::EditCheckpoint) {
		if (GuiButton({ uiArea.x + 20, 230, 170, 30 }, "Edit checkpoint")) {
			m_Mode = Mode::EditCheckpoint;
		}
		GuiSetState(STATE_NORMAL);
		if (GuiButton({ uiArea.x + 210, 230, 170, 30 }, "Edit track")) {
			m_Mode = Mode::EditTrack;
		}
	}
	else {
		if (GuiButton({ uiArea.x + 210, 230, 170, 30 }, "Edit track")) {
			m_Mode = Mode::EditTrack;
		}
		GuiSetState(STATE_NORMAL);
		if (GuiButton({ uiArea.x + 20, 230, 170, 30 }, "Edit checkpoint")) {
			m_Mode = Mode::EditCheckpoint;
		}
	}

	// =================== Drop List ==========================
	Rectangle listBounds = { uiArea.x + 20, 325, 360, 180 };
	DrawText("Select File to Load:", uiArea.x + 20, 310, 10, DARKGRAY);
	int previousListActive = m_ListActive;

	GuiListView(listBounds, m_FileListString.c_str(), &m_ListScrollIndex, &m_ListActive);

	if (m_ListActive != previousListActive && m_ListActive >= 0 && m_ListActive < static_cast<int>(m_Files.count)) {
		std::string selectedFilePath = m_Files.paths[m_ListActive];

		size_t lastSlash = selectedFilePath.find_last_of("/\\");
		std::string fileName = (lastSlash == std::string::npos) ? selectedFilePath : selectedFilePath.substr(lastSlash + 1);

		TextCopy(m_TextBuffer, fileName.c_str());

		m_Track->Load(fileName);

		TextCopy(m_ListStatusMessage, TextFormat("Loaded: %s", fileName.c_str()));
		m_ListStatusColor = BLUE;
		m_ListStatusTimer = 3.0f;
	}

	if (m_ListStatusTimer > 0.0f) {
		DrawText(m_ListStatusMessage, uiArea.x + 200, 310, 10, m_ListStatusColor);
	}

	if (GuiButton({ uiArea.x + 310, listBounds.y + listBounds.height + 10, 70, 30 }, "Delete File")) {
		if (m_ListActive >= 0 && m_ListActive < (int)m_Files.count) {
			std::string fileToDelete = m_Files.paths[m_ListActive];

			std::error_code ec;
			if (std::filesystem::remove(fileToDelete, ec)) {
				TextCopy(m_ListStatusMessage, "File deleted successfully!");
				m_ListStatusColor = DARKGREEN;
			}
			else {
				TextCopy(m_ListStatusMessage, "Error: Could not delete file.");
				m_ListStatusColor = RED;
			}
			m_ListStatusTimer = 3.0f;

			ScanTrackFile();
			m_ListActive = -1;
		}
		else {
			TextCopy(m_ListStatusMessage, "Error: No file selected to delete!");
			m_ListStatusColor = RED;
			m_ListStatusTimer = 3.0f;
		}
	}
}

void EditState::Reset() {
	if (!m_Track->ShowingCheckpoints()) {
		m_Track->ToggleShowCheckpoints();
	}
}

void EditState::ScanTrackFile() {
	if (m_Files.count > 0) {
		UnloadDirectoryFiles(m_Files);
	}

	m_Files = LoadDirectoryFiles("app/train/tracks/");

	m_FileListString.clear();
	for (unsigned int i = 0; i < m_Files.count; i++) {
		std::string fullPath(m_Files.paths[i]);
		size_t lastSlash = fullPath.find_last_of("/\\");
		std::string fileName = (lastSlash == std::string::npos) ? fullPath : fullPath.substr(lastSlash + 1);

		m_FileListString += fileName;
		if (i < m_Files.count - 1) {
			m_FileListString += ";";
		}
	}

	if (m_Files.count == 0) {
		m_FileListString = "No files found";
	}
}