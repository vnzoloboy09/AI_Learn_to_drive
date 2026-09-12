#include "Utils.h"

float CastRay(Vector2 carPos, float rayAngle, float maxRange, Track& track) {
    float stepSize = 4.0f;
    float currentDistance = 0.0f;

    while (currentDistance < maxRange) {
        currentDistance += stepSize;
        float checkX = carPos.x + cos(rayAngle) * currentDistance;
        float checkY = carPos.y + sin(rayAngle) * currentDistance;

        if (track.IsWall(checkX, checkY)) {
            return currentDistance / maxRange;
        }
    }
    return 1.0f;
    return 1.0f;
}

float DegreeToRadian(float angle) {
    return angle * 3.14 / 180.0f;
}