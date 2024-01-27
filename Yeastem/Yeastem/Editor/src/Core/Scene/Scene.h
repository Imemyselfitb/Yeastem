#pragma once

#include "Core/OpenGL/Include.h"
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

public:
	uint64_t CurrentTime = 0;

private:
	void UpdateScripts(int shapeIdx, float deltaTime);

private:
	LuaState m_LuaState;
	std::vector<Shape> m_Shapes;
	Yeastem::Renderer m_Renderer;
};

YEASTEM_END

