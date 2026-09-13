#pragma once

#include <raylib.h>
#include <vector>

class Track
{
public:
	Track();
	~Track();

	void Load(const char* filePath);
	void Unload();
	bool IsWall(float x, float y) const;
	bool CheckCarPassedCheckpoint(Vector2 carPos, size_t& currentCheckpointIndex) const;
	bool IsLastCheckpoint(size_t& currentCheckpointIndex) const;
	void Render() const;

public:
	std::vector<Vector2> checkpoints;
	Texture2D trackTexture;
	Image trackImage;

private:
};

