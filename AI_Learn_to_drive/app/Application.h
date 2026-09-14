#pragma once

#include "game/Car.h"
#include "game/Track.h"
#include "train/Genetic.h"
#include "game/state/State.h"

#include <unordered_map>
#include <memory>

enum class Mode {
	Train = 0,
	Edit,
	Demo
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

	Vector2 GetMousePos() const { return m_MousePos; }
	void SetState(StateType type);

private:
	void RegistryState(StateType type, std::unique_ptr<State> state);

private:
	// Application
	const int m_ScreenWidth;
	const int m_ScreenHeight;

	// Input
	Vector2 m_MousePos = { 0.0f, 0.0f };
	Track m_Track;

	std::unordered_map<StateType, std::unique_ptr<State>> m_StateRegistry;
	State* currentState = nullptr;
};

