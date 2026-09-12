#pragma once

#include <raylib.h>
#include "app/model/Network.h"
#include "Utils.h"

const float TURN_RATE = 2.5f;
const float ACCELERATION_RATE = 400.0f;
const float MAX_FORWARD_SPEED = 250.0f;
const float MAX_REVERSE_SPEED = -80.0f;
const float FRICTION = 0.93f;
const float CAR_WIDTH = 30.0f;
const float CAR_HEIGHT = 16.0f;
const Vector2 START_POSITION = { 170, 470 };

class Track;

class Car
{
public:
	Car(bool isManual);
	~Car();

	void Update(float dt, Track& track);
	void Render() const;

	void SetDefault();

	Vector2 GetPosition() const { return m_Position; }
	void SetPosition(Vector2 position) { m_Position = position; }
	float GetAngle() const { return m_Angle; }
	void SetAngle(float angle) { m_Angle = angle; }
	float GetSpeed() const { return m_Speed; }
	void SetSpeed(float speed) { m_Speed = speed; }
	bool IsAlive() const { return m_IsAlive; }
	int GetFitness() const { return m_Fitness; }
	void SetFitness(int fitness) { m_Fitness = fitness; }

private:
	void HandleInput();
	void ApplySteeringAndAcceleration(float dt);
	void UpdatePosition(float dt);
	void UpdateFitness();
	void CheckBounds(Track& track);
	void UpdateRay(Track& track);

private:
	Network brain = Network({ 5, 10, 10, 2 });
	Vector2 m_Position = START_POSITION;
	float m_Angle = DegreeToRadian(-90.0f);
	float m_Speed = 0.0f;
	bool m_IsAlive = true;
	int m_Fitness = 0;
	bool m_IsManual;

	float m_Steering = 0.0f;
	float m_Acceleration = 0.0f;

	Vector2 m_RayEnd1 = { 0.0f, 0.0f };
	Vector2 m_RayEnd2 = { 0.0f, 0.0f };
	Vector2 m_RayEnd3 = { 0.0f, 0.0f };
	Vector2 m_RayEnd4 = { 0.0f, 0.0f };
	Vector2 m_RayEnd5 = { 0.0f, 0.0f };
};

