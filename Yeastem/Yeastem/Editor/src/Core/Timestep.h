#pragma once

YEASTEM_BEGIN

class Timestep
{
public:
	Timestep(uint64_t millis) :m_Milliseconds(millis) {}
	Timestep() { this->m_Milliseconds = SDL_GetTicks64(); }

	~Timestep() {}

	float getSeconds() { this->m_Milliseconds / 1000.0; }
	float getMilliseconds() { this->m_Milliseconds; }

private:
	uint64_t m_Milliseconds;
};

YEASTEM_END
