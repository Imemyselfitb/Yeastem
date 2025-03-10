#pragma once

YEASTEM_BEGIN

class EventHandler
{
public:
	using EventListenerCallback = std::function<void(SDL_Event&)>;

public:
	EventHandler() {}
	~EventHandler() {}

	void AddEventListener(uint32_t type, EventListenerCallback fn);
	void AddEventListener(SDL_Event& evt, EventListenerCallback fn);

	void Dispatch(SDL_Event& evt);

	bool IsKeyDown(uint32_t keyCode);

private:
	std::unordered_map<uint32_t, std::vector<EventListenerCallback>> m_EventListeners;
	std::unordered_map<uint32_t, bool> m_KEYS;
};

YEASTEM_END
