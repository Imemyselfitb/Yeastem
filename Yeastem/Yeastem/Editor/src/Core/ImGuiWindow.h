#pragma once

YEASTEM_BEGIN

class ImGuiWindow
{
public:
	using ShowFunction = void(*)();

public:
	ImGuiWindow() {}
	ImGuiWindow(ShowFunction showfunc) : m_Show_function(showfunc) {}
	~ImGuiWindow() {}

public:
	void AttachRenderFunctionality(ShowFunction showfunc)
	{
		m_Show_function = showfunc;
	}

	void Render()
	{
		m_Show_function();
	}
	
private:
	ShowFunction m_Show_function;
};

YEASTEM_END
