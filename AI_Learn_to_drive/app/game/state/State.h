#pragma once

enum class StateType {
	Train = 0,
	Edit,
	Demo
};

class State {
public:
	State() = default;
	~State() = default;

	virtual void HandleInput() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render() = 0;
	virtual void RenderUI() = 0;
	virtual void Reset() = 0;
public:
	enum {
		Train,
		Edit,
		Demo
	};
};

class Track;

class TrackAwareState : public State {
public:
	explicit TrackAwareState(Track* track) : m_Track(track) {}

protected:
	Track* m_Track;

};