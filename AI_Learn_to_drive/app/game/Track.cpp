#include "Track.h"

#include <raymath.h>
#include <iostream>

Track::Track() 
{

}

void Track::Load(const char* filePath) {
    m_TrackImage = LoadImage(filePath);
    m_TrackTexture = LoadTextureFromImage(m_TrackImage);

    for (int y = 0; y < m_TrackImage.height; y++) {
        for (int x = 0; x < m_TrackImage.width; x++) {
            Color pixel = GetImageColor(m_TrackImage, x, y);

            if (pixel.r <= 50 && pixel.g <= 50 && pixel.b >= 205) {
                Vector2 newCp = { (float)x, (float)y };

                bool tooClose = false;
                for (const auto& cp : m_Checkpoints) {
                    if (Vector2Distance(cp, newCp) < 20.0f) { 
                        tooClose = true;
                        break;
                    }
                }

                if (!tooClose) {
                    m_Checkpoints.push_back(newCp);
                }
            }
        }
    }

    std::cout << "Load track with: " << m_Checkpoints.size() << " checkpoints\n";
}

void Track::Unload() {
    UnloadImage(m_TrackImage);
    UnloadTexture(m_TrackTexture);
}

bool Track::IsWall(float x, float y) const {
    if (x < 0 || x >= m_TrackImage.width || y < 0 || y >= m_TrackImage.height) {
        return true;
    }
    Color pixelColor = GetImageColor(m_TrackImage, (int)x, (int)y);

    return (pixelColor.r < 50 && pixelColor.g < 50 && pixelColor.b < 50);
}

bool Track::CheckCarPassedCheckpoint(Vector2 carPos, size_t& currentCheckpointIndex) const {
    if (m_Checkpoints.empty()) {
        return false;
    }
    
    size_t nextIndex = (currentCheckpointIndex + 1) % m_Checkpoints.size();
    Vector2 targetCp = m_Checkpoints[nextIndex];

    float distance = Vector2Distance(carPos, targetCp);

    if (distance < 60.0f) {
        currentCheckpointIndex = nextIndex;
        return true;
    }

    return false;
}

bool Track::IsLastCheckpoint(size_t& currentCheckpointIndex) const {
    return currentCheckpointIndex == m_Checkpoints.size() - 1;
}

void Track::Render() const {
    DrawTexture(m_TrackTexture, 0, 0, WHITE);
    for (auto& checkpoint : m_Checkpoints) {
        DrawCircle(static_cast<int>(checkpoint.x), static_cast<int>(checkpoint.y), 50.0f, YELLOW);
    }
}