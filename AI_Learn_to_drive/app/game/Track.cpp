#include "Track.h"

Track::Track() 
{

}

void Track::Load(const char* filePath) {
    m_TrackImage = LoadImage(filePath);
    m_TrackTexture = LoadTextureFromImage(m_TrackImage);
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

void Track::Render() const {
    DrawTexture(m_TrackTexture, 0, 0, WHITE);
}