#pragma once

#include <vector>

#include "State.h"
#include "app/game/Car.h"
#include "app/train/Genetic.h"

class TrainState : public TrackAwareState {
public:
	TrainState(Track* track);
	~TrainState();

	void HandleInput() override;
	void Update(float dt) override;
	void Render() const override;

private:
	bool IsAllDead();

private:
	float m_Timer = 0.0f;

	std::vector<Car> m_Cars;
	Car m_User;

	Genetic m_Ga;
	size_t m_GenerationCount = 0;

	float m_BestScore = FLT_MIN;
};

