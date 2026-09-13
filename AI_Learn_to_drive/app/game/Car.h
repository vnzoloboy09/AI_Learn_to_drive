#pragma once

#include <raylib.h>
#include "app/model/Network.h"
#include "Utils.h"

const float TURN_RATE = 2.5f;
const float ACCELERATION_RATE = 380.0f;
const float MAX_FORWARD_SPEED = 220.0f;
const float MAX_REVERSE_SPEED = -80.0f;
const float FRICTION = 0.93f;
const float CAR_WIDTH = 30.0f;
const float CAR_HEIGHT = 16.0f;
const Vector2 START_POSITION = { 170, 470 };
const float START_ANGLE = DegreeToRadian(-90);
const float MAX_RAY_RANGE = 150.0f;

class Track;

class Car
{
public:
	Car(bool isManual);
	~Car();

	void Update(float dt, Track& track);
	void Render() const;

	void Reset(Vector2 spawnPoint = START_POSITION, float spawnAngle = START_ANGLE);

	Vector2 GetPosition() const { return m_Position; }
	void SetPosition(Vector2 position) { m_Position = position; }
	float GetAngle() const { return m_Angle; }
	void SetAngle(float angle) { m_Angle = angle; }
	float GetSpeed() const { return m_Speed; }
	void SetSpeed(float speed) { m_Speed = speed; }
	bool IsAlive() const { return m_IsAlive; }
	float GetFitness() const { return m_Fitness; }
	void SetFitness(float fitness) { m_Fitness = fitness; }
	Network GetBrain() const { return m_Brain; }
	void SetBrain(Network brain) { m_Brain = brain; }

	float GetDistanceTravel() const { return m_DistanceTravel; }

private:
	void HandleInput();
	void ApplySteeringAndAcceleration(float dt);
	void UpdatePosition(float dt);
	void UpdateFitness(Track& track);
	void CheckBounds(Track& track);
	void UpdateRay(Track& track);

private:
	Network m_Brain = Network({ 5, 10, 10, 2 });
	Vector2 m_Position = START_POSITION;
	float m_Angle = START_ANGLE;
	float m_Speed = 0.0f;
	bool m_IsAlive = true;
	float m_Fitness = 0.0f;
	bool m_IsManual;

	float m_Steering = 0.0f;
	float m_Acceleration = 0.0f;

	float m_DistanceTravel = 0.0f;
	size_t m_CheckpointPassed = 0;
	float m_Timer = 0.0f;

	std::vector<Vector2> m_RayEnds;
};

