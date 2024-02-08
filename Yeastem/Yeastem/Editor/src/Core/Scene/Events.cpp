#include "yst_pch.h"

#include "Events.h"

YEASTEM_BEGIN

void EventHandler::AddEventListener(uint32_t type, EventListenerCallback fn)
{
	if (this->m_EventListeners.find(type) == this->m_EventListeners.end())
	{
		this->m_EventListeners[type] = { fn };
	}
	this->m_EventListeners[type].push_back(fn);
}

void EventHandler::AddEventListener(SDL_Event& evt, EventListenerCallback fn)
{
	this->AddEventListener(evt.type, fn);
}

void EventHandler::Dispatch(SDL_Event& evt)
{
	if (evt.type == SDL_KEYUP)
	{
		this->m_KEYS[evt.key.keysym.sym] = false;
	}
	else if (evt.type == SDL_KEYDOWN && !evt.key.repeat)
	{
		this->m_KEYS[evt.key.keysym.sym] = true;
	}

	if (this->m_EventListeners.find(evt.type) == this->m_EventListeners.end()) return;
	for (auto& fn : this->m_EventListeners.at(evt.type))
		fn(evt);
}

bool EventHandler::IsKeyDown(unsigned int key)
{
	if (this->m_KEYS.find(key) == this->m_KEYS.end()) return false;
	return this->m_KEYS.at(key);
}

YEASTEM_END
