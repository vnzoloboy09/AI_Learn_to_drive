#include "Track.h"

#include <raymath.h>
#include <iostream>

Track::Track() 
{

}

Track::~Track() {
    Unload();
}

void Track::Load(const char* filePath) {
    trackImage = LoadImage(filePath);
    trackTexture = LoadTextureFromImage(trackImage);

    //for (int y = 0; y < trackImage.height; y++) {
    //    for (int x = 0; x < trackImage.width; x++) {
    //        Color pixel = GetImageColor(trackImage, x, y);

    //        if (pixel.r >= 205 && pixel.g >= 205 && pixel.b <= 50) {
    //            Vector2 newCp = { (float)x, (float)y };

    //            bool tooClose = false;
    //            for (const auto& cp : checkpoints) {
    //                if (Vector2Distance(cp, newCp) <= 62.0f) {
    //                    tooClose = true;
    //                    break;
    //                }
    //            }

    //            if (!tooClose) {
    //                checkpoints.push_back(newCp);
    //            }
    //        }
    //    }
    //}
}

void Track::Unload() {
    UnloadTexture(trackTexture);
    UnloadImage(trackImage);
}

bool Track::IsWall(float x, float y) const {
    if (x < 0 || x >= trackImage.width || y < 0 || y >= trackImage.height) {
        return true;
    }
    Color pixelColor = GetImageColor(trackImage, (int)x, (int)y);

    return (pixelColor.r < 50 && pixelColor.g < 50 && pixelColor.b < 50);
}

bool Track::CheckCarPassedCheckpoint(Vector2 carPos, size_t currentCheckpointIndex) const {
    if (checkpoints.empty()) {
        return false;
    }

    float distance = Vector2Distance(carPos, checkpoints[currentCheckpointIndex]);

    if (distance < CHECKPOINT_SIZE) {
        return true;
    }

    return false;
}

bool Track::IsLastCheckpoint(size_t& currentCheckpointIndex) const {
    return currentCheckpointIndex == checkpoints.size() - 1;
}

float Track::GetAngelToCheckpoint(Vector2 carPos, Vector2 centerRayEnd, size_t checkpointIndex) const {
    if (checkpointIndex >= checkpoints.size()) {
        return 0.0f;
    }

    Vector2 cpPos = checkpoints[checkpointIndex];

    Vector2 forward = { centerRayEnd.x - carPos.x, centerRayEnd.y - carPos.y };

    Vector2 toCheckpoint = { cpPos.x - carPos.x, cpPos.y - carPos.y };

    float dot = forward.x * toCheckpoint.x + forward.y * toCheckpoint.y;
    float det = forward.x * toCheckpoint.y - forward.y * toCheckpoint.x;

    float angleRad = atan2f(det, dot);

    return fabs(angleRad * (180.0f / PI));

}

void Track::Render() const {
    DrawTexture(trackTexture, 0, 0, WHITE);
}