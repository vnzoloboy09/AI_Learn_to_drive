#pragma once

#include <raylib.h>
#include <vector>

class Track
{
public:
	Track();
	~Track() = default;

	void Load(const char* filePath);
	void Unload();
	bool IsWall(float x, float y) const;
	bool CheckCarPassedCheckpoint(Vector2 carPos, size_t& currentCheckpointIndex) const;
	bool IsLastCheckpoint(size_t& currentCheckpointIndex) const;
	void Render() const;

private:
	Image m_TrackImage;
	Texture2D m_TrackTexture;

	std::vector<Vector2> m_Checkpoints;
};

