#include "Car.h"

Car::Car(bool isMaual) 
    : m_IsManual(isMaual)
{

}

Car::~Car() {

}

void Car::Update() {
    if (!m_IsAlive) 
        return;

    float steering = 0.0f;
    float acceleration = 0.0f;

    if (m_IsManual) {
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    acceleration = 1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  acceleration = -0.5f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  steering = -1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) steering = 1.0f;
    }
    else {
        std::vector<float> inputs = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
        brain.Forward(inputs);
        std::vector<float> outputs = brain.GetOutput();
        steering = (float)outputs[0];
        acceleration = (float)outputs[1];
    }

    m_Angle += steering * 0.04f * (m_Speed != 0 ? 1.0f : 0.0f);

    if (acceleration > 0) {
        m_Speed += 0.1f;
        if (m_Speed > 4.0f) m_Speed = 4.0f;
    }
    else if (acceleration < 0) {
        m_Speed -= 0.1f;
        if (m_Speed < -1.5f) m_Speed = -1.5f;
    }
    else {
        m_Speed *= 0.95f;
    }

    m_Position.x += cos(m_Angle) * m_Speed;
    m_Position.y += sin(m_Angle) * m_Speed;

    m_Fitness++;

    if (m_Position.x < 50 || m_Position.x > 750 || m_Position.y < 50 || m_Position.y > 550) {
        m_IsAlive = false;
    }
}

void Car::Render() const {
    Color carColor = (m_IsAlive ? GREEN : RED);
    Rectangle rect = { m_Position.x, m_Position.y, 30.0f, 16.0f };

    // Set the origin to the center of the rectangle so it spins on its middle
    Vector2 origin = { rect.width / 2.0f, rect.height / 2.0f };

    // Draw the rotated rectangle
    DrawRectanglePro(rect, origin, m_Angle * RAD2DEG, carColor);
}

void Car::SetDefault() {
    m_Position = { 200, 200 };
    m_Angle = 0.0f;
    m_Speed = 0.0f;
    m_IsAlive = true;
    m_Fitness = 0;
}