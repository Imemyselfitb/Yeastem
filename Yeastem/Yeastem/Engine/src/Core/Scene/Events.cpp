#include "yst_pch.h"

#include "Events.h"

YEASTEM_BEGIN

void EventHandler::AddEventListener(uint32_t type, EventListenerCallback fn)
{
	if (m_EventListeners.find(type) == m_EventListeners.end())
	{
		m_EventListeners[type] = { fn };
	}
	m_EventListeners[type].push_back(fn);
}

void EventHandler::AddEventListener(SDL_Event& evt, EventListenerCallback fn)
{
	AddEventListener(evt.type, fn);
}

void EventHandler::Dispatch(SDL_Event& evt)
{
	if (evt.type == SDL_KEYUP)
	{
		m_KEYS[evt.key.keysym.sym] = false;
	}
	else if (evt.type == SDL_KEYDOWN && !evt.key.repeat)
	{
		m_KEYS[evt.key.keysym.sym] = true;
	}

	if (m_EventListeners.find(evt.type) == m_EventListeners.end()) return;
	for (auto& fn : m_EventListeners.at(evt.type))
		fn(evt);
}

bool EventHandler::IsKeyDown(uint32_t key)
{
	if (m_KEYS.find(key) == m_KEYS.end()) return false;
	return m_KEYS.at(key);
}

YEASTEM_END
