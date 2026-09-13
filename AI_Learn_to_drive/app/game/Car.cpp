#include "Car.h"

#include <raymath.h>

Car::Car(bool isMaual) 
    : m_IsManual(isMaual), m_RayEnds(5)
{
    m_RayEnds = {
        { 0.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 0.0f },
        { 0.0f, 0.0f }
    };
}

Car::~Car() {

}

void Car::Update(float dt, float timer, Track& track) {
    if (!m_IsAlive) 
        return;

    timer += dt;

    UpdateRay(track);
    HandleInput();
    ApplySteeringAndAcceleration(dt);
    UpdatePosition(dt);
    UpdateFitness(timer);
    CheckBounds(track);
}

void Car::Render() const {
    // Render car
    Color carColor = (m_IsAlive ? GREEN : RED);
    Rectangle rect = { m_Position.x, m_Position.y, CAR_WIDTH, CAR_HEIGHT };
    Vector2 origin = { rect.width / 2.0f, rect.height / 2.0f };
    DrawRectanglePro(rect, origin, m_Angle * RAD2DEG, carColor);

    // Render ray
    DrawLineV(m_Position, m_RayEnds[0], BLUE);
    DrawLineV(m_Position, m_RayEnds[1], BLUE);
    DrawLineV(m_Position, m_RayEnds[2], BLUE);
    DrawLineV(m_Position, m_RayEnds[3], BLUE);
    DrawLineV(m_Position, m_RayEnds[4], BLUE);
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
        std::vector<float> inputs = {
            Vector2Distance(m_RayEnds[0], m_Position) / MAX_RAY_RANGE,
            Vector2Distance(m_RayEnds[1], m_Position) / MAX_RAY_RANGE,
            Vector2Distance(m_RayEnds[2], m_Position) / MAX_RAY_RANGE,
            Vector2Distance(m_RayEnds[3], m_Position) / MAX_RAY_RANGE,
            Vector2Distance(m_RayEnds[4], m_Position) / MAX_RAY_RANGE,
        };
        m_Brain.Forward(inputs);
        std::vector<float> outputs = m_Brain.GetOutput();
        steering = (float)outputs[0];
        acceleration = (float)outputs[1];
    }

    m_Acceleration = acceleration;
    m_Steering = steering;
}

void Car::ApplySteeringAndAcceleration(float dt) {
    if (m_Speed != 0.0f) {
        m_Angle += m_Steering * TURN_RATE * dt;
    }

    if (m_Acceleration > 0) {
        m_Speed += ACCELERATION_RATE * dt;
        if (m_Speed > MAX_FORWARD_SPEED) {
            m_Speed = MAX_FORWARD_SPEED;
        }
    }
    else if (m_Acceleration < 0) {
        m_Speed -= ACCELERATION_RATE * dt;
        if (m_Speed < MAX_REVERSE_SPEED) {
            m_Speed = MAX_REVERSE_SPEED;
        }
    }
    else {
        m_Speed *= std::pow(FRICTION, dt * 60.0f);
    }
}

void Car::UpdatePosition(float dt) {
    Vector2 newPos = {
        m_Position.x + cos(m_Angle) * m_Speed * dt,
        m_Position.y + sin(m_Angle) * m_Speed * dt
    };

    m_DistanceTravel = Vector2Distance(m_Position, newPos);
    m_Position = newPos;
}

void Car::UpdateFitness(float timer) {
    if (!m_IsAlive) {
        return;
    }

    if (timer > 3.0f && m_DistanceTravel < 100.0f) {
        m_Fitness -= 50.0f;
    }

    m_Fitness = m_DistanceTravel + timer;
}

void Car::CheckBounds(Track& track) {
    float hw = CAR_WIDTH / 2.0f;
    float hh = CAR_HEIGHT / 2.0f;

    Vector2 localCorners[4] = {
        { -hw, -hh },
        { hw, -hh },
        { hw,  hh },
        { -hw,  hh }
    };

    float c = cos(m_Angle);
    float s = sin(m_Angle);

    for (int i = 0; i < 4; ++i) {
        float worldX = m_Position.x + (localCorners[i].x * c - localCorners[i].y * s);
        float worldY = m_Position.y + (localCorners[i].x * s + localCorners[i].y * c);

        if (track.IsWall(worldX, worldY)) {
            m_IsAlive = false;
            return;
        }
    }

    if (track.IsWall(m_Position.x, m_Position.y)) {
        m_IsAlive = false;
    }
}

void Car::UpdateRay(Track& track) {
    float angle1 = m_Angle - 60.0f * DEG2RAD;
    float angle2 = m_Angle - 30.0f * DEG2RAD;
    float angle3 = m_Angle;
    float angle4 = m_Angle + 30.0f * DEG2RAD;
    float angle5 = m_Angle + 60.0f * DEG2RAD;

    float dist1 = CastRay(m_Position, angle1, MAX_RAY_RANGE, track);
    float dist2 = CastRay(m_Position, angle2, MAX_RAY_RANGE, track);
    float dist3 = CastRay(m_Position, angle3, MAX_RAY_RANGE, track);
    float dist4 = CastRay(m_Position, angle4, MAX_RAY_RANGE, track);
    float dist5 = CastRay(m_Position, angle5, MAX_RAY_RANGE, track);

    m_RayEnds[0] = { m_Position.x + cos(angle1) * (dist1 * MAX_RAY_RANGE), m_Position.y + sin(angle1) * (dist1 * MAX_RAY_RANGE) };
    m_RayEnds[1] = { m_Position.x + cos(angle2) * (dist2 * MAX_RAY_RANGE), m_Position.y + sin(angle2) * (dist2 * MAX_RAY_RANGE) };
    m_RayEnds[2] = { m_Position.x + cos(angle3) * (dist3 * MAX_RAY_RANGE), m_Position.y + sin(angle3) * (dist3 * MAX_RAY_RANGE) };
    m_RayEnds[3] = { m_Position.x + cos(angle4) * (dist4 * MAX_RAY_RANGE), m_Position.y + sin(angle4) * (dist4 * MAX_RAY_RANGE) };
    m_RayEnds[4] = { m_Position.x + cos(angle5) * (dist5 * MAX_RAY_RANGE), m_Position.y + sin(angle5) * (dist5 * MAX_RAY_RANGE) };
}

void Car::Reset(Vector2 spawnPoint, float spawnAngle) {
    m_Position = spawnPoint;
    m_Angle = spawnAngle;
    m_Speed = 0.0f;
    m_IsAlive = true;
    m_Fitness = 0;
    m_DistanceTravel = 0.0f;
}