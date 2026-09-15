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
};

