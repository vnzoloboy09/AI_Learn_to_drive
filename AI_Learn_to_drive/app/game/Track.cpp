#include "Track.h"

#include <raymath.h>
#include <iostream>
#include <fstream>

Track::Track() 
{
}

Track::~Track() {
    Unload();
}

void Track::Save(const std::string& filepath) const {
    std::ofstream outFile(filepath, std::ios::binary);
    if (!outFile.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open file for writing: %s", filepath.c_str());
        return;
    }

    size_t cpCount = checkpoints.size();
    outFile.write(reinterpret_cast<const char*>(&cpCount), sizeof(cpCount));
    if (cpCount > 0) {
        outFile.write(reinterpret_cast<const char*>(checkpoints.data()), cpCount * sizeof(Vector2));
    }

    int fileSize = 0;
    unsigned char* fileData = ExportImageToMemory(trackImage, ".png", &fileSize);

    if (fileData != nullptr && fileSize > 0) {
        outFile.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
        outFile.write(reinterpret_cast<const char*>(fileData), fileSize);

        MemFree(fileData);
        TraceLog(LOG_INFO, "Track and checkpoints successfully saved to single file: %s", filepath.c_str());
    }
    else {
        TraceLog(LOG_ERROR, "Failed to export track image to memory!");
    }

    outFile.close();
}

void Track::Load(const std::string& filepath) {
    std::ifstream inFile(filepath, std::ios::binary);
    if (!inFile.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open file for reading: %s", filepath.c_str());
        return;
    }

    size_t cpCount = 0;
    inFile.read(reinterpret_cast<char*>(&cpCount), sizeof(cpCount));

    checkpoints.resize(cpCount);
    if (cpCount > 0) {
        inFile.read(reinterpret_cast<char*>(checkpoints.data()), cpCount * sizeof(Vector2));
    }

    int fileSize = 0;
    inFile.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));

    if (fileSize > 0) {
        std::vector<unsigned char> fileBuffer(fileSize);
        inFile.read(reinterpret_cast<char*>(fileBuffer.data()), fileSize);

        UnloadImage(trackImage);
        trackImage = LoadImageFromMemory(".png", fileBuffer.data(), fileSize);

        UnloadTexture(trackTexture);
        trackTexture = LoadTextureFromImage(trackImage);

        TraceLog(LOG_INFO, "Loaded track and %zu checkpoints from single file.", checkpoints.size());
    }
    else {
        TraceLog(LOG_ERROR, "Invalid or empty image data in file!");
    }

    inFile.close();
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

    if (!m_ShowCheckpoints) {
        return;
    }

    for (int i = 0; i < checkpoints.size(); i++) {
        DrawCircle(static_cast<int>(checkpoints[i].x),
            static_cast<int>(checkpoints[i].y), CHECKPOINT_SIZE, YELLOW);
        std::string id = std::to_string(i);
        DrawText(id.c_str(), static_cast<int>(checkpoints[i].x - 5),
            static_cast<int>(checkpoints[i].y) - 5, 20, BLACK);
    }
}