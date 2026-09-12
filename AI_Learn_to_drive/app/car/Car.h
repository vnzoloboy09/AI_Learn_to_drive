#pragma once

#include <raylib.h>
#include "app/model/Network.h"

class Car
{
public:
	Car(bool isManual);
	~Car();

	void Update(float dt);
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
	void CheckBounds();
	void UpdateRay();

private:
	Network brain = Network({ 5, 10, 10, 2 });
	Vector2 m_Position = { 200.0f, 200.0f };
	float m_Angle = 0.0f;
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

