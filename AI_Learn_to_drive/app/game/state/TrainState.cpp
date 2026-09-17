#include "TrainState.h"

#include "app/Application.h"
#include "app/model/Help.h"

#include <raygui.h>
#include <vector>
#include <iostream>
#include <raymath.h>
#include <format>

TrainState::TrainState(Track* track, Application& app)
	: TrackAwareState(track), m_UserCar(-1, true), m_App(app)
{
	bool loaded = m_Ga.LoadPopulation("app/train/cars", m_GenerationCount, m_Cars, START_POSITION, START_ANGLE);

	if (!loaded) {
		m_GenerationCount = 1;
		for (int i = 0; i < 110; ++i) {
			m_Cars.emplace_back(i);
		}
	}

	Rectangle uiArea = m_App.GetUIArea();
	nnArea = {
		uiArea.x + 20,
		uiArea.y + 195,
		uiArea.width - 40,
		uiArea.height - 170
	};

	std::vector<Layer> layers = m_Cars[0].GetBrain().GetLayers();
	int numLayers = layers.size() + 1;
	float rowHeightStep = nnArea.height / (numLayers + 1);
	m_NodePositions.resize(numLayers);

	int numNeuronsInRow = layers[0].inputCnt;
	int preNumNeuronsInRow;
	float colWidthStep = nnArea.width / (numNeuronsInRow + 1);
	int connectionCount = 0;

	for (int n = 0; n < numNeuronsInRow; n++) {
		float x = nnArea.x + (n + 1) * colWidthStep;
		float y = nnArea.y + 1 * rowHeightStep;
		m_NodePositions[0].push_back({ x, y });
	}

	for (int r = 1; r < numLayers; r++) {
		preNumNeuronsInRow = numNeuronsInRow;
		numNeuronsInRow = layers[r - 1].outputCnt;
		connectionCount += numNeuronsInRow * preNumNeuronsInRow;
		colWidthStep = nnArea.width / (numNeuronsInRow + 1);

		for (int n = 0; n < numNeuronsInRow; n++) {
			float x = nnArea.x + (n + 1) * colWidthStep;
			float y = nnArea.y + (r + 1) * rowHeightStep;
			m_NodePositions[r].push_back({ x, y });
		}
	}

	m_BlinkConnection.resize(connectionCount);
	for (auto& c : m_BlinkConnection) {
		c.first = RandomFloat(0.0f, 2.5f);
	}
}

TrainState::~TrainState() {
}

void TrainState::HandleInput() {
	Vector2 mousePos = m_App.GetMousePos();
	if (IsKeyPressed(KEY_H)) {
		m_Track->ToggleShowCheckpoints();
	}
	if (IsKeyPressed(KEY_R)) {
		m_UserCar.Reset();
	}
	if (IsKeyPressed(KEY_E)) {
		m_App.SetState(StateType::Edit);
	}
	if (IsKeyPressed(KEY_M)) {
		m_App.SetState(StateType::Demo);
	}

	if (mousePos.x < m_App.GetSimArea().width && mousePos.y < m_App.GetSimArea().height &&
		(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) ||
		IsMouseButtonPressed(MOUSE_BUTTON_LEFT)))
	{
		m_SelectedCarId = -1;
		for (int i = 0; i < m_Cars.size(); i++) {
			if (IsPointInCar(mousePos, m_Cars[i].GetPosition(), CAR_HEIGHT, CAR_HEIGHT, m_Cars[i].GetAngle())) {
				m_SelectedCarId = i;
				break;
			}
		}
		if (m_SelectedCarId == -1) {
			m_SelectedCheckpointId = -1;
			int totalCheckpoints = m_Track->checkpoints.size();
			for (int i = 0; i < totalCheckpoints; i++) {
				if (Vector2Distance(mousePos, m_Track->checkpoints[i]) <= CHECKPOINT_SIZE) {
					m_SelectedCheckpointId = i;
					break;
				}
			}
		}
	}
}

void TrainState::Update(float dt) {
	m_Timer += dt;
	for (auto& car : m_Cars) {
		car.Update(dt, *m_Track);
	}
	m_UserCar.Update(dt, *m_Track);

	if (IsAllDead() || m_Timer > m_EpochTime) {
		m_GenerationCount++;
		m_BestScore = m_Ga.Evolve(m_Cars, START_POSITION, START_ANGLE);

		m_Timer = 0.0f;

		if (m_GenerationCount > 0 && m_GenerationCount % 20 == 0) {
			m_Ga.SavePopulation("app/train/cars", m_GenerationCount, m_Cars);
			m_SaveStatusTimer = 3.0f;
		}
	}

	m_SaveStatusTimer = std::max(m_SaveStatusTimer - dt, 0.0f);

	for (auto& c : m_BlinkConnection) {
		if (c.first > 0.0f) {
			c.first -= dt;
		}
		else if (rand() % 100 == 0) {
			c.first = 7.0f;
			c.second = {
				static_cast<unsigned char>(GetRandomValue(0, 255)),
				static_cast<unsigned char>(GetRandomValue(0, 255)),
				static_cast<unsigned char>(GetRandomValue(0, 255)),
				255
			};
		}
	}
}

void TrainState::Render() {
	Rectangle simArea = m_App.GetSimArea();
	BeginScissorMode(static_cast<int>(simArea.x), static_cast<int>(simArea.y),
		static_cast<int>(simArea.width), static_cast<int>(simArea.height));
	m_Track->Render();
	Color color = m_Cars[0].IsAlive() ? PINK : DARKBLUE;
	m_Cars[0].SetColor(color);
	for (int i = m_Cars.size() - 1; i >= 0; i--) {
		m_Cars[i].Render();
	}

	m_UserCar.Render();
	EndScissorMode();

	RenderUI();
}

void TrainState::RenderUI() {
	Rectangle uiArea = m_App.GetUIArea();

	DrawText(TextFormat("Gen: %zu", (m_GenerationCount)), 10, 10, 20, DARKGRAY);
	DrawText(TextFormat("Time: %.3f ms", m_Timer), 10, 40, 20, DARKGRAY);
	DrawText(TextFormat("Best: %.3f", m_BestScore), 10, 70, 20, DARKGRAY);

	GuiPanel(uiArea, "Control Panel");

	if (GuiButton({ uiArea.x + 20, 40, 170, 35 }, "Edit [E]")) {
		m_App.SetState(StateType::Edit);
	}
	if (GuiButton({ uiArea.x + 210, 40, 170, 35 }, "Watch Demo [M]")) {
		m_App.SetState(StateType::Demo);
	}
	if (GuiButton({ uiArea.x + 20, 85, 170, 35 }, "Save population")) {
		m_Ga.SavePopulation("app/train/cars", m_GenerationCount, m_Cars);
		m_SaveStatusTimer = 3.0f;
	}
	if (GuiButton({ uiArea.x + 210, 85, 170, 35 }, "Reset user car [R]")) {
		m_UserCar.Reset();
	}
	if (GuiValueBox({ m_App.GetUIArea().x + 20, 130, 170, 35 }, nullptr, &m_EpochTime, 1, 3600, m_EpochEditing)) {
		m_EpochEditing = !m_EpochEditing;
	}
	DrawText("Set Epoch Time", uiArea.x + 20, 170, 10, DARKGRAY);
	if (m_Track->ShowingCheckpoints()) {
		GuiSetState(STATE_PRESSED);
		if (GuiButton({ uiArea.x + 210, 130, 170, 35 }, "Toggle checkpoints [H]")) {
			m_Track->ToggleShowCheckpoints();
		}
		GuiSetState(STATE_NORMAL);
	}
	else {
		if (GuiButton({ uiArea.x + 210, 130, 170, 35 }, "Toggle checkpoints [H]")) {
			m_Track->ToggleShowCheckpoints();
		}
	}

	if (m_SaveStatusTimer > 0.0f) {
		DrawText("Saved", uiArea.x + 20, 125, 10, BLUE);
	}

	DrawRectangleRec(nnArea, Fade(DARKGRAY, 0.1f));
	DrawRectangleLinesEx(nnArea, 1, Fade(DARKGRAY, 0.3f));

	if (m_SelectedCarId != -1) {
		RenderSelectedCar();
	}
	else {
		if (m_SelectedCheckpointId == -1) {
			RenderNeuralNetwork();
		}
		else {
			RenderSelectedCheckPoint();
		}
	}
}

void TrainState::RenderNeuralNetwork() {
	int connectionID = -1;
	for (int r = 0; r < m_NodePositions.size() - 1; r++) {
		for (size_t i = 0; i < m_NodePositions[r].size(); i++) {
			for (size_t j = 0; j < m_NodePositions[r + 1].size(); j++) {
				connectionID++;
				if (m_BlinkConnection[connectionID].first < 6.5f) {
					DrawLineEx(m_NodePositions[r][i], m_NodePositions[r + 1][j], 1.5f, BLUE);
				}
				else {
					DrawLineEx(m_NodePositions[r][i], m_NodePositions[r + 1][j], 3.5f,
						m_BlinkConnection[connectionID].second);
				}
			}
		}
	}

	for (int r = 0; r < m_NodePositions.size(); r++) {
		for (size_t n = 0; n < m_NodePositions[r].size(); n++) {
			Vector2 pos = m_NodePositions[r][n];
			DrawCircleV(pos, 8.0f, DARKBLUE);
		}
	}
}

void TrainState::RenderSelectedCheckPoint() {
	DrawCircle(m_App.GetUIArea().x + 100, 300, 50, YELLOW);
	DrawCircleLines(m_App.GetUIArea().x + 100, 300, 50, BLACK);
	DrawText(std::to_string(m_SelectedCheckpointId).c_str(), m_App.GetUIArea().x + 90, 285, 30, BLACK);
	
	std::string info = "X: " + std::to_string(static_cast<int>(m_Track->checkpoints[m_SelectedCheckpointId].x));
	info += "\nY: " + std::to_string(static_cast<int>(m_Track->checkpoints[m_SelectedCheckpointId].y));
	DrawText(info.c_str(), m_App.GetUIArea().x + 70, 370, 20, BLACK);

	if (GuiValueBox({ m_App.GetUIArea().x + 190, 275, 100, 35 }, "ID: ", &m_ChangedId, 0, 100, m_CheckpointEditing)) {
		m_CheckpointEditing = !m_CheckpointEditing;
	}

	if (GuiButton({ m_App.GetUIArea().x + 190, 320, 100, 35 }, "Change ID")) {
		if (m_ChangedId < m_Track->checkpoints.size()) {
			std::swap(m_Track->checkpoints[m_ChangedId], m_Track->checkpoints[m_SelectedCheckpointId]);
		}
	}
}

void TrainState::RenderSelectedCar() {
	static float updateTimer = 3.0f;
	static float UIangle = 0.0f;
	static float fitness = 0.0f;
	static float speed = 0.0f;
	static float angle = 0.0f;
	static Vector2 position;
	int centerX = m_App.GetUIArea().x + 200;
	int centerY = 300;
	int width = CAR_WIDTH * 3;
	int height = CAR_HEIGHT * 3;
	Rectangle rect = { centerX, centerY, width, height };
	Vector2 origin = { width / 2, height / 2 };

	updateTimer -= 1.0f / 60.0f;
	if (updateTimer <= 0) {
		fitness = m_Cars[m_SelectedCarId].GetFitness();
		angle = m_Cars[m_SelectedCarId].GetAngle();
		speed = m_Cars[m_SelectedCarId].GetSpeed();
		position = m_Cars[m_SelectedCarId].GetPosition();
		updateTimer = 3.0f;
	}

	UIangle += 1.0f;
	DrawRectanglePro(rect, origin, UIangle, m_Cars[m_SelectedCarId].GetColor());
	DrawText(std::to_string(m_SelectedCarId).c_str(), centerX - 8, centerY - 10, 20, BLACK);
	std::string info = std::format(
		"FitNess:   {:.3f}\n"
		"	 Angle:   {:.3f}\n"
		"	Speed:   {:.3f}\n"
		"Position:  {:.3f} - {:.3f}\n",
		fitness,
		angle,
		speed,
		position.x,
		position.y
	);

	DrawText(info.c_str(), centerX - 150, centerY + 100, 20, DARKGRAY);
}

void TrainState::Reset() {
	for (auto& car : m_Cars) {
		car.Reset();
	}
	m_UserCar.Render();
	m_Timer = 0.0f;
}

bool TrainState::IsAllDead() {
	for (auto& car : m_Cars) {
		if (car.IsAlive()) {
			return false;
		}
	}
	return true;
}

bool TrainState::IsPointInCar(Vector2 point, Vector2 carCenter, 
	float width, float height, float angleDegrees) 
{
	float dx = point.x - carCenter.x;
	float dy = point.y - carCenter.y;
	
	float rad = -angleDegrees * DEG2RAD;
	float localX = dx * cosf(rad) - dy * sinf(rad);
	float localY = dx * sinf(rad) + dy * cosf(rad);

	return (fabsf(localX) <= width / 2.0f) && (fabsf(localY) <= height / 2.0f);
}