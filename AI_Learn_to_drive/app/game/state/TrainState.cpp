#include "TrainState.h"

#include "app/Application.h"
#include "app/model/Help.h"

#include <raygui.h>
#include <vector>

TrainState::TrainState(Track* track, Application& app)
	: TrackAwareState(track), m_UserCar(true), m_App(app)
{
	bool loaded = m_Ga.LoadPopulation("app/train/cars", m_GenerationCount, m_Cars, START_POSITION, START_ANGLE);

	if (!loaded) {
		m_GenerationCount = 1;
		for (int i = 0; i < 110; ++i) {
			m_Cars.emplace_back(false);
		}
	}

	Rectangle uiArea = m_App.GetUIArea();
	nnArea = {
		uiArea.x + 20,
		uiArea.y + 150,
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
	if (IsKeyPressed(KEY_H)) {
		m_Track->ToggleShowCheckpoints();
	}
}

void TrainState::Update(float dt) {
	m_Timer += dt;
	for (auto& car : m_Cars) {
		car.Update(dt, *m_Track);
	}
	m_UserCar.Update(dt, *m_Track);

	if (IsAllDead() || m_Timer > 35.0f) {
		m_GenerationCount++;
		m_BestScore = m_Ga.Evolve(m_Cars, START_POSITION, START_ANGLE);

		m_Timer = 0.0f;
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
	for (auto& car : m_Cars) {
		car.Render();
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

	if (GuiButton({ uiArea.x + 20, 40, 170, 35 }, "Edit")) {
		m_App.SetState(StateType::Edit);
	}
	if (GuiButton({ uiArea.x + 210, 40, 170, 35 }, "Watch Demo")) {
		m_App.SetState(StateType::Demo);
	}
	if (GuiButton({ uiArea.x + 20, 85, 170, 35 }, "Save population")) {
		m_Ga.SavePopulation("app/train/cars", m_GenerationCount, m_Cars);
		m_SaveStatusTimer = 3.0f;
	}
	if (GuiButton({ uiArea.x + 210, 85, 170, 35 }, "Reset user car")) {
		m_UserCar.Reset();
	}

	if (m_SaveStatusTimer > 0.0f) {
		DrawText("Saved", uiArea.x + 20, 125, 10, BLUE);
	}

	DrawRectangleRec(nnArea, Fade(DARKGRAY, 0.1f));
	DrawRectangleLinesEx(nnArea, 1, Fade(DARKGRAY, 0.3f));

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