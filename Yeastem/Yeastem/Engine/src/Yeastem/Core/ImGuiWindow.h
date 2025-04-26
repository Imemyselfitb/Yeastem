#pragma once

YEASTEM_BEGIN

class ImGuiWindow
{
public:
	using ShowFunction = std::function<void(bool&)>;

public:
	ImGuiWindow(ShowFunction showfunc, bool canClose = true) 
		: m_Show_function(showfunc), m_canClose(canClose) {}
	~ImGuiWindow() {}

public:
	void Render() { if (m_IsOpen || !m_canClose) m_Show_function(m_IsOpen); }
	
private:
	ShowFunction m_Show_function = ShowFunction();
	bool m_IsOpen = true;
	bool m_canClose = true;
};

YEASTEM_END
