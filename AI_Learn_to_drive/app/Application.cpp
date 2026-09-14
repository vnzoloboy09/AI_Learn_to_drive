#include "Application.h"

#include "game/state/StateInclude.h"

Application::Application(int screenWidth, int screenHeight, const char* title)
	: m_ScreenWidth(screenWidth), m_ScreenHeight(screenHeight)
{
	InitWindow(m_ScreenWidth, m_ScreenHeight, title);
	SetTargetFPS(60);

	RegistryState(StateType::Train, std::make_unique<TrainState>(&m_Track, *this));
	RegistryState(StateType::Edit,  std::make_unique<EditState>(&m_Track, *this));
	RegistryState(StateType::Demo,  std::make_unique<DemoState>(&m_Track, *this));
	SetState(StateType::Train);

	m_Track.Load("app/train/tracks/track_01"); //default track
}

Application::~Application() {
	CloseWindow();
}

void Application::Run() {
	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		HandleInput();
		Update(dt);
		Render();
	}
} 

void Application::HandleInput() {
	m_MousePos = GetMousePosition();
	currentState->HandleInput();
}

void Application::Update(float dt) {
	currentState->Update(dt);
}

void Application::Render() const {
	BeginDrawing();
	ClearBackground(DARKGRAY);

	currentState->Render();

	EndDrawing();
}

void Application::RegistryState(StateType type, std::unique_ptr<State> state) {
	m_StateRegistry[type] = std::move(state);
}

void Application::SetState(StateType type) {
	auto it = m_StateRegistry.find(type);

	if (it != m_StateRegistry.end()) {
		currentState = it->second.get();
		currentState->Reset();
	}
}
