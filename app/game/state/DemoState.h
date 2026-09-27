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
	void Render() override;
	void RenderUI() override;
	void Reset() override;

	void ResetCar();

private:
	Car m_Car;

	Application& m_App;
};

