#pragma once

#include <raylib.h>
#include <vector>

const size_t CHECKPOINT_SIZE = 30.0f;

class Track
{
public:
	Track();
	~Track();

	void Load(const char* filePath);
	void Unload();
	bool IsWall(float x, float y) const;
	bool CheckCarPassedCheckpoint(Vector2 carPos, size_t currentCheckpointIndex) const;
	bool IsLastCheckpoint(size_t& currentCheckpointIndex) const;
	float GetAngelToCheckpoint(Vector2 carPos, Vector2 centerRayEnd, size_t checkpointIndex) const;
	void Render() const;

public:
	std::vector<Vector2> checkpoints;
	Texture2D trackTexture;
	Image trackImage;

private:
	const char* DEFAULT_PATH = "save";
};

