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

bool Track::CheckCarPassedCheckpoint(Vector2 carPos, size_t& currentCheckpointIndex) const {
    if (checkpoints.empty()) {
        return false;
    }
    
    size_t nextIndex = (currentCheckpointIndex + 1) % checkpoints.size();
    Vector2 targetCp = checkpoints[nextIndex];

    float distance = Vector2Distance(carPos, targetCp);

    if (distance <= 60.0f) {
        currentCheckpointIndex = nextIndex;
        return true;
    }

    return false;
}

bool Track::IsLastCheckpoint(size_t& currentCheckpointIndex) const {
    return currentCheckpointIndex == checkpoints.size() - 1;
}

void Track::Render() const {
    DrawTexture(trackTexture, 0, 0, WHITE);
}