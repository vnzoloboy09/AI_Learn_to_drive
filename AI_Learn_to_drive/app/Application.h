#pragma once

#include "game/Track.h"
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
	Application();
	~Application();

	void Run();
	void HandleInput();
	void Update(float dt);
	void Render() const;

	Vector2 GetMousePos() const { return m_MousePos; }
	void SetState(StateType type);
	Rectangle GetSimArea() const { return m_SimArea; }
	Rectangle GetUIArea() const { return m_UIArea; }

private:
	void RegistryState(StateType type, std::unique_ptr<State> state);

private:
	Rectangle m_SimArea = { 0, 0, 1280, 960 };
	Rectangle m_UIArea = { 1280, 0, 400, 960 };

	Vector2 m_MousePos = { 0.0f, 0.0f };
	Track m_Track;

	std::unordered_map<StateType, std::unique_ptr<State>> m_StateRegistry;
	State* currentState = nullptr;
};

