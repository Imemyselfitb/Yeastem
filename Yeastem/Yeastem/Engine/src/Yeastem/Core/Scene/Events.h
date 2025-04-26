#pragma once

YEASTEM_BEGIN

class EventHandler
{
public:
	EventHandler() {}
	~EventHandler() {}

	bool IsKeyDown(uint32_t keyCode) const;
	void Dispatch(SDL_Event& evt);

private:
	std::unordered_map<uint32_t, bool> m_KEYS;
};

YEASTEM_END
