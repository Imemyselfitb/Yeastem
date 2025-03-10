#pragma once

YEASTEM_BEGIN

class Timestep
{
public:
	Timestep(uint64_t millis) :m_Milliseconds(millis) {}
	Timestep() { m_Milliseconds = SDL_GetTicks64(); }

	~Timestep() {}

	float getSeconds() { m_Milliseconds / 1000.0; }
	float getMilliseconds() { m_Milliseconds; }

private:
	uint64_t m_Milliseconds;
};

YEASTEM_END
