#pragma once

#include "Core/OpenGL/Include.h"
#include "Events.h"
#include "LuaAPI/Lua.h"

YEASTEM_BEGIN

class Scene
{
public:
	Scene() {}
	~Scene() {}

	void AddObject(Shape& shape);
	void Lua_Init();
	void Lua_AttachCFunction(int(*nativeFunction)(lua_State*), const char* functionName);
	void Lua_ExcecuteScript(const std::string& file);
	void Update(float DeltaTime, int WindowWidth, int WindowHeight);
	void Render();
	void ProcessEvent(SDL_Event& evt);

public:
	uint64_t CurrentTime = 0;
	Yeastem::EventHandler m_EventHandler;

private:
	void UpdateObjectFromScripts(int shapeIdx);
	void UpdateScriptsFromObjects(int shapeIdx);
	void AddKeyEnum(const char* keyName, unsigned int keyCode);

private:
	int m_ScriptCount = 0;
	LuaState m_LuaState;
	std::vector<Shape> m_Shapes;
	Yeastem::Renderer m_Renderer;
};

YEASTEM_END

