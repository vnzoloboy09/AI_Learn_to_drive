#pragma once

#include "State.h"
#include "app/game/Car.h"

class Application;

class DemoState : public TrackAwareState
{
public:
	DemoState(Track* track, Application& app);
	~DemoState();

	void HandleInput() override;
	void Update(float dt) override;
	void Render() const override;
	void Reset() override;

private:
	Car m_Car;

	Application& m_App;
};

