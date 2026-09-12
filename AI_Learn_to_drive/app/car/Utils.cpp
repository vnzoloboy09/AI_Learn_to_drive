#include "Utils.h"

float CastRay(Vector2 carPos, float rayAngle, float maxRange) {
    float stepSize = 5.0f;
    float currentDistance = 0.0f;
    while (currentDistance < maxRange) {
        currentDistance += stepSize;
        float checkX = carPos.x + cos(rayAngle) * currentDistance;
        float checkY = carPos.y + sin(rayAngle) * currentDistance;
        if (checkX < 50.0f || checkX > 750.0f || checkY < 50.0f || checkY > 550.0f) {
            return currentDistance / maxRange;
        }
    }
    return 1.0f;
}