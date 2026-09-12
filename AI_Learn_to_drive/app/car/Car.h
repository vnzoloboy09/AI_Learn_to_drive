#pragma once

#include <raylib.h>
#include "app/model/Network.h"

class Car
{
public:
	Car(bool isManual);
	~Car();

	void Update();
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
	Vector2 m_Position = { 200, 200 };
	float m_Angle = 0.0f;
	float m_Speed = 0.0f;
	bool m_IsAlive = true;
	int m_Fitness = 0;

	bool m_IsManual;

	Network brain = Network({ 5, 10, 10, 2 });
};

