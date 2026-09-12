#include "Car.h"

#include "Utils.h"

Car::Car(bool isMaual) 
    : m_IsManual(isMaual)
{

}

Car::~Car() {

}

void Car::Update() {
    if (!m_IsAlive) 
        return;

    UpdateRay();
    HandleInput();
    ApplySteeringAndAcceleration();
    UpdatePosition();
    UpdateFitness();
    CheckBounds();
}

void Car::Render() const {
    // Render car
    Color carColor = (m_IsAlive ? GREEN : RED);
    Rectangle rect = { m_Position.x, m_Position.y, 30.0f, 16.0f };
    Vector2 origin = { rect.width / 2.0f, rect.height / 2.0f };
    DrawRectanglePro(rect, origin, m_Angle * RAD2DEG, carColor);

    // Render ray
    DrawLineV(m_Position, m_RayEnd1, YELLOW);
    DrawLineV(m_Position, m_RayEnd2, YELLOW);
    DrawLineV(m_Position, m_RayEnd3, YELLOW);
    DrawLineV(m_Position, m_RayEnd4, YELLOW);
    DrawLineV(m_Position, m_RayEnd5, YELLOW);
}

void Car::HandleInput() {
    float acceleration = 0.0f;
    float steering = 0.0f;
    
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

    m_Acceleration = acceleration;
    m_Steering = steering;
}

void Car::ApplySteeringAndAcceleration() {
    m_Angle += m_Steering * 0.04f * (m_Speed != 0 ? 1.0f : 0.0f);

    if (m_Acceleration > 0) {
        m_Speed += 0.1f;
        if (m_Speed > 4.0f) m_Speed = 4.0f;
    }
    else if (m_Acceleration < 0) {
        m_Speed -= 0.1f;
        if (m_Speed < -1.5f) m_Speed = -1.5f;
    }
    else {
        m_Speed *= 0.95f;
    }
}

void Car::UpdatePosition() {
    m_Position.x += cos(m_Angle) * m_Speed;
    m_Position.y += sin(m_Angle) * m_Speed;
}

void Car::UpdateFitness() {
    m_Fitness++; // TODO: change latter
}

void Car::CheckBounds() {
    if (m_Position.x < 50 || m_Position.x > 750 || m_Position.y < 50 || m_Position.y > 550) {
        m_IsAlive = false;
    }
}

void Car::UpdateRay() {
    float maxSensorRange = 150.0f;

    float angle1 = m_Angle - 60.0f * DEG2RAD;
    float angle2 = m_Angle - 30.0f * DEG2RAD;
    float angle3 = m_Angle;
    float angle4 = m_Angle + 30.0f * DEG2RAD;
    float angle5 = m_Angle + 60.0f * DEG2RAD;

    float dist1 = CastRay(m_Position, angle1, maxSensorRange);
    float dist2 = CastRay(m_Position, angle2, maxSensorRange);
    float dist3 = CastRay(m_Position, angle3, maxSensorRange);
    float dist4 = CastRay(m_Position, angle4, maxSensorRange);
    float dist5 = CastRay(m_Position, angle5, maxSensorRange);

    m_RayEnd1 = { m_Position.x + cos(angle1) * (dist1 * maxSensorRange), m_Position.y + sin(angle1) * (dist1 * maxSensorRange) };
    m_RayEnd2 = { m_Position.x + cos(angle2) * (dist2 * maxSensorRange), m_Position.y + sin(angle2) * (dist2 * maxSensorRange) };
    m_RayEnd3 = { m_Position.x + cos(angle3) * (dist3 * maxSensorRange), m_Position.y + sin(angle3) * (dist3 * maxSensorRange) };
    m_RayEnd4 = { m_Position.x + cos(angle4) * (dist4 * maxSensorRange), m_Position.y + sin(angle4) * (dist4 * maxSensorRange) };
    m_RayEnd5 = { m_Position.x + cos(angle5) * (dist5 * maxSensorRange), m_Position.y + sin(angle5) * (dist5 * maxSensorRange) };
}

void Car::SetDefault() {
    m_Position = { 200, 200 };
    m_Angle = 0.0f;
    m_Speed = 0.0f;
    m_IsAlive = true;
    m_Fitness = 0;
}