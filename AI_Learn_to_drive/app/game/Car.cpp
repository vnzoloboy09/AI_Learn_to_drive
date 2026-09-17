#include "Car.h"

#include <raymath.h>
#include <iostream>
#include <algorithm>
#include <string>

Car::Car(int id, bool isManual)
    : m_IsManual(isManual), m_RayEnds(5), m_Brain({ 6, 10, 10, 2 }), m_ID(id)
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

void Car::Update(float dt, Track& track) {
    if (!m_IsAlive) {
        m_Color = RED;
        return;
    }

    UpdateRay(track);
    HandleInput();
    ApplySteeringAndAcceleration(dt);
    UpdatePosition(dt);
    CheckBounds(track);
    UpdateFitness(dt, track);
}

void Car::Render() const {
    // Render car
    Rectangle rect = { m_Position.x, m_Position.y, CAR_WIDTH, CAR_HEIGHT };
    Vector2 origin = { rect.width / 2.0f, rect.height / 2.0f };
    DrawRectanglePro(rect, origin, m_Angle * RAD2DEG, m_Color);

    // Render ray
    //DrawLineV(m_Position, m_RayEnds[0], BLUE);
    //DrawLineV(m_Position, m_RayEnds[1], BLUE);
    //DrawLineV(m_Position, m_RayEnds[2], BLUE);
    //DrawLineV(m_Position, m_RayEnds[3], BLUE);
    //DrawLineV(m_Position, m_RayEnds[4], BLUE);
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
            m_Speed / MAX_FORWARD_SPEED
        };
        m_Brain.Forward(inputs);
        std::vector<float> outputs = m_Brain.GetOutput();
        steering = outputs[0];
        acceleration = outputs[1];
    }

    m_Acceleration = acceleration;
    m_Steering = steering;
}

void Car::ApplySteeringAndAcceleration(float dt) {
    if (m_Speed != 0.0f) {
        m_Angle += m_Steering * TURN_RATE * FIXED_DT;
    }

    if (m_Acceleration != 0.0f) {
        m_Speed += m_Acceleration * ACCELERATION_RATE * FIXED_DT;
    }
    m_Speed *= std::pow(FRICTION, FIXED_DT * 60.0f);

    m_Speed = std::clamp(m_Speed, 0.0f, MAX_FORWARD_SPEED);
    if (m_Speed < 0.1f) m_Speed = 0.0f;
}

void Car::UpdatePosition(float dt) {
    Vector2 newPos = {
        m_Position.x + cos(m_Angle) * m_Speed * dt,
        m_Position.y + sin(m_Angle) * m_Speed * dt
    };

    m_Position = newPos;
}

void Car::UpdateFitness(float dt, Track& track) {
    if (m_Demoing || m_IsManual) {
        return;
    }

    m_CheckpointTimer += dt;
    if (m_CheckpointTimer > TIME_OUT) {
        m_IsAlive = false;
    }

    if (!m_IsAlive) {
        return;
    }

    size_t totalCheckpoint = track.checkpoints.size();

    if (m_LapFinished == 0) {
        m_Fitness -= 0.01f * dt;
    }

    float straightRay = Vector2Distance(m_Position, m_RayEnds[2]) / MAX_RAY_RANGE;
    if (m_Speed > 0.4f) {
        m_Fitness += m_Speed * 1.5f * dt;
    }
    else if (straightRay > 0.3) {
        m_Fitness -= 3.0f * dt;
    }

    float outerRaysDiff = std::abs(Vector2Distance(m_RayEnds[0], m_Position) -
        Vector2Distance(m_RayEnds[4], m_Position)) / MAX_RAY_RANGE;
    m_Fitness -= 0.5f * outerRaysDiff * dt;

    if (totalCheckpoint > 0) {
        float currentDisToTargetCheckPoint = Vector2Distance(m_Position, track.checkpoints[m_TargetCheckpointID]);

        m_Fitness += (m_DisToTargetCheckPoint - currentDisToTargetCheckPoint) * 2.0f;
        m_DisToTargetCheckPoint = currentDisToTargetCheckPoint;

        if (m_DisToTargetCheckPoint <= CHECKPOINT_SIZE) {
            m_Fitness += (TIME_OUT - m_CheckpointTimer) * 10.0f;
            if (track.IsLastCheckpoint(m_TargetCheckpointID)) {
                m_LapFinished++;
                m_Fitness += 1000.0f;
            }
            m_CheckpointTimer = 0.0f;
            m_TargetCheckpointID = (m_TargetCheckpointID + 1) % totalCheckpoint;
            m_DisToTargetCheckPoint = Vector2Distance(m_Position, track.checkpoints[m_TargetCheckpointID]);
        }
    }
}

void Car::CheckBounds(Track& track) {
    float hw = CAR_WIDTH / 2.0f;
    float hh = CAR_HEIGHT / 2.0f;

    Vector2 localPoints[9] = {
        { -hw, -hh },{ hw, -hh },
        { hw,  hh },{ -hw,  hh },
        { -hw, 0.0f },{ hw, 0.0f },
        { 0.0f, -hh },{ 0.0f,  hh },
        { 0.0f, 0.0f }
    };

    float c = cos(m_Angle);
    float s = sin(m_Angle);

    for (const auto& pt : localPoints) {
        float worldX = m_Position.x + (pt.x * c - pt.y * s);
        float worldY = m_Position.y + (pt.x * s + pt.y * c);

        if (track.IsWall(worldX, worldY)) {
            m_IsAlive = false;
            return;
        }
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
    m_Fitness = 0.0f;
    m_CheckpointTimer = 0.0f;
    m_TargetCheckpointID = 0;
    m_LapFinished = 0;
    m_Color = GREEN;
}