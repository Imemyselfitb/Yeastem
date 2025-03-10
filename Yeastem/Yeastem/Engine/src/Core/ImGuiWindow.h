#pragma once

YEASTEM_BEGIN

class ImGuiWindow
{
public:
	using ShowFunction = std::function<void(bool&)>;

public:
	ImGuiWindow() {}
	ImGuiWindow(ShowFunction showfunc) : m_Show_function(showfunc) {}
	~ImGuiWindow() {}

public:
	void AttachRenderFunctionality(ShowFunction showfunc)
	{
		if(m_isShown)
			m_Show_function = showfunc;
	}

	void Render()
	{
		m_Show_function(m_isShown);
	}
	
private:
	ShowFunction m_Show_function = ShowFunction();
	bool m_isShown = true;
};

YEASTEM_END
