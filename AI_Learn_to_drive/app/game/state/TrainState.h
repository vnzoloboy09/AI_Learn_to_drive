#pragma once

#include <vector>

#include "State.h"
#include "app/game/Car.h"
#include "app/train/Genetic.h"

class Application;

class TrainState : public TrackAwareState {
public:
	TrainState(Track* track, Application& app);
	~TrainState();

	void HandleInput() override;
	void Update(float dt) override;
	void Render() override;
	void RenderUI() override;
	void Reset() override;

private:
	bool IsAllDead();
	void RenderNeuralNetwork();
	void RenderSelectedCheckPoint();
	void RenderSelectedCar();

	bool IsPointInCar(Vector2 point, Vector2 carCenter, float width, float height, float angleDegrees);

private:
	float m_Timer = 0.0f;
	std::vector<Car> m_Cars;
	Car m_UserCar;
	Genetic m_Ga;
	size_t m_GenerationCount = 0;
	float m_BestScore = FLT_MIN;
	
	Application& m_App;

	char m_SaveStatusMessage[128] = "";
	float m_SaveStatusTimer = 0.0f;

	char m_EpochText[16] = "30";
	bool m_EpochEditing = false;
	int m_EpochTime = 30.0f;

	int m_SelectedCheckpointId = -1;
	int m_ChangedId = -1;
	bool m_CheckpointEditing = false;

	int m_SelectedCarId = -1;

	Rectangle nnArea;
	std::vector<std::vector<Vector2>> m_NodePositions;
	std::vector<std::pair<float, Color>> m_BlinkConnection;
};

