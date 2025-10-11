#pragma once

#include "Yeastem/Core/Scene/Components.h"

YEASTEM_BEGIN

class ProjectManagerPanel
{
public:
	enum class State
	{
		ProjectCreation,
		ProjectSaving
	};

public:
	void Open(State state);
	void Render(bool& isOpen);

public:
	State m_ProjectState = State::ProjectCreation;
	bool m_IsOpen = false;
};

YEASTEM_END
