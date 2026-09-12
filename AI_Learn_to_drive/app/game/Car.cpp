#include "Car.h"

Car::Car(bool isMaual) 
    : m_IsManual(isMaual)
{

}

Car::~Car() {

}

void Car::Update(float dt, Track& track) {
    if (!m_IsAlive) 
        return;

    UpdateRay(track);
    HandleInput();
    ApplySteeringAndAcceleration(dt);
    UpdatePosition(dt);
    UpdateFitness();
    CheckBounds(track);
}

void Car::Render() const {
    // Render car
    Color carColor = (m_IsAlive ? GREEN : RED);
    Rectangle rect = { m_Position.x, m_Position.y, CAR_WIDTH, CAR_HEIGHT };
    Vector2 origin = { rect.width / 2.0f, rect.height / 2.0f };
    DrawRectanglePro(rect, origin, m_Angle * RAD2DEG, carColor);

    // Render ray
    DrawLineV(m_Position, m_RayEnd1, BLUE);
    DrawLineV(m_Position, m_RayEnd2, BLUE);
    DrawLineV(m_Position, m_RayEnd3, BLUE);
    DrawLineV(m_Position, m_RayEnd4, BLUE);
    DrawLineV(m_Position, m_RayEnd5, BLUE);
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
    m_Position.x += cos(m_Angle) * m_Speed * dt;
    m_Position.y += sin(m_Angle) * m_Speed * dt;
}

void Car::UpdateFitness() {
    m_Fitness++; // TODO: change latter
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
    float maxSensorRange = 150.0f;

    float angle1 = m_Angle - 60.0f * DEG2RAD;
    float angle2 = m_Angle - 30.0f * DEG2RAD;
    float angle3 = m_Angle;
    float angle4 = m_Angle + 30.0f * DEG2RAD;
    float angle5 = m_Angle + 60.0f * DEG2RAD;

    float dist1 = CastRay(m_Position, angle1, maxSensorRange, track);
    float dist2 = CastRay(m_Position, angle2, maxSensorRange, track);
    float dist3 = CastRay(m_Position, angle3, maxSensorRange, track);
    float dist4 = CastRay(m_Position, angle4, maxSensorRange, track);
    float dist5 = CastRay(m_Position, angle5, maxSensorRange, track);

    m_RayEnd1 = { m_Position.x + cos(angle1) * (dist1 * maxSensorRange), m_Position.y + sin(angle1) * (dist1 * maxSensorRange) };
    m_RayEnd2 = { m_Position.x + cos(angle2) * (dist2 * maxSensorRange), m_Position.y + sin(angle2) * (dist2 * maxSensorRange) };
    m_RayEnd3 = { m_Position.x + cos(angle3) * (dist3 * maxSensorRange), m_Position.y + sin(angle3) * (dist3 * maxSensorRange) };
    m_RayEnd4 = { m_Position.x + cos(angle4) * (dist4 * maxSensorRange), m_Position.y + sin(angle4) * (dist4 * maxSensorRange) };
    m_RayEnd5 = { m_Position.x + cos(angle5) * (dist5 * maxSensorRange), m_Position.y + sin(angle5) * (dist5 * maxSensorRange) };
}

void Car::SetDefault() {
    m_Position = START_POSITION;
    m_Angle = 0.0f;
    m_Speed = 0.0f;
    m_IsAlive = true;
    m_Fitness = 0;
}