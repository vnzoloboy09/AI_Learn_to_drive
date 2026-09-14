#pragma once

#include <raylib.h>
#include <vector>
#include <string>

const size_t CHECKPOINT_SIZE = 30.0f;

class Track
{
public:
	Track();
	~Track();

	void Load(const std::string& filePath);
	void Unload();
	bool IsWall(float x, float y) const;
	bool CheckCarPassedCheckpoint(Vector2 carPos, size_t currentCheckpointIndex) const;
	bool IsLastCheckpoint(size_t& currentCheckpointIndex) const;
	float GetAngelToCheckpoint(Vector2 carPos, Vector2 centerRayEnd, size_t checkpointIndex) const;
	void ToggleShowCheckpoints() { m_ShowCheckpoints = !m_ShowCheckpoints; }
	bool ShowingCheckpoints() const { return m_ShowCheckpoints; }
	void Render() const;

	void Save(const std::string& filepath) const;

public:
	std::vector<Vector2> checkpoints;
	Texture2D trackTexture;
	Image trackImage;

private:
	bool m_ShowCheckpoints;
};

