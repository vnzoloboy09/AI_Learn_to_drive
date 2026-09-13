#pragma once

#include "game/Car.h"
#include "game/Track.h"
#include "train/Genetic.h"

enum class Mode {
	Train = 0,
	Edit,
};

enum class EditMode {
	TrackEdit = 0,
	CheckpointEdit
};

class Application
{
public:
	Application(int screenWidth = 1280, int screenHeight = 960, const char* title = "title");
	~Application();

	void Run();
	void HandleInput();
	void Update(float dt);
	void Render() const;

	bool IsAllDead();

	void Reset();

private:
	void HandelTrainInput();
	void HandelEditInput();
	
	void UpdateTrain(float dt);
	void UpdateEdit();

	void RenderCar() const;
	void RenderUI() const;
	void RenderCheckpoints() const;

private:
	// Application
	const int m_ScreenWidth;
	const int m_ScreenHeight;
	bool m_Running;
	Mode m_Mode = Mode::Train;
	EditMode m_EditMode = EditMode::TrackEdit;
	float m_Timer = 0.0f;
	size_t m_GenerationCount;
	float m_BestScore = 0.0f;

	// Input
	Vector2 m_MousePos = { 0.0f, 0.0f };
	
	// Edit Mode
	int m_SelectedCheckpointId = -1;

	// Game
	std::vector<Car> m_Cars;
	Car user = Car(true);
	Genetic m_Ga;
	Track m_Track;
};

