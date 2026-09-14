#pragma once

class State {
public:
	State() = default;
	~State() = default;

	virtual void HandleInput() = 0;
	virtual void Update(float dt) = 0;
	virtual void Render() const = 0;
public:
	enum {
		Train,
		Edit,
		Demo
	};
};

class Track;

class TrackAwareState : State {
public:
	explicit TrackAwareState(Track* track) : m_Track(track) {}

protected:
	Track* m_Track;

};