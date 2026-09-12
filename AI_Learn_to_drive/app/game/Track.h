#pragma once

#include <raylib.h>

class Track
{
public:
	Track();
	~Track() = default;

	void Load(const char* filePath);
	void Unload();
	bool IsWall(float x, float y) const;
	void Render() const;

private:
	Image m_TrackImage;
	Texture2D m_TrackTexture;
};

