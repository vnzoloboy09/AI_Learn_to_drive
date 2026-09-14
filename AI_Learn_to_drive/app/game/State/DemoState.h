#pragma once

#include "State.h"
#include "app/game/Car.h"

class DemoState : TrackAwareState
{
public:
	DemoState(Track* track);
	~DemoState();

	void HandleInput() override;
	void Update(float dt) override;
	void Render() const override;

private:
	Car m_Car;
};

