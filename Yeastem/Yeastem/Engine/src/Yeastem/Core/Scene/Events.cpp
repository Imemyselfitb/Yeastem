#include "yst_pch.h"

#include "Events.h"

YEASTEM_BEGIN

void EventHandler::Dispatch(SDL_Event& evt)
{
	if (evt.type == SDL_EVENT_KEY_UP)
		m_KEYS[evt.key.key] = false;
	else if (evt.type == SDL_EVENT_KEY_DOWN && !evt.key.repeat)
		m_KEYS[evt.key.key] = true;
}

bool EventHandler::IsKeyDown(uint32_t key) const
{
	if (m_KEYS.find(key) == m_KEYS.end()) return false;
	return m_KEYS.at(key);
}

YEASTEM_END
