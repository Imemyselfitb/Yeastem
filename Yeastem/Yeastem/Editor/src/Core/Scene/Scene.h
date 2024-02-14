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
	void Update(float DeltaTime);
	void Render();
	void ProcessEvent(SDL_Event& evt);

	void CreateFrameBuffer(unsigned int width, unsigned int height);
	void RecreateFrameBuffer(unsigned int width, unsigned int height);

	unsigned int GetFrameBufferColourAttachmentID() const { return this->m_FrameBuffer->GetColourAttachmentID(); }
	unsigned int GetFrameBufferDepthAttachmentID() const { return this->m_FrameBuffer->GetDepthAttachmentID(); }

public:
	LuaState m_LuaState;
	uint64_t CurrentTime = 0;
	Yeastem::EventHandler m_EventHandler;
	ImVec2 SceneSize;

private:
	void UpdateObjectFromScripts(int shapeIdx);
	void UpdateScriptsFromObjects(int shapeIdx);
	void AddKeyEnum(const char* keyName, unsigned int keyCode);

private:
	int m_ScriptCount = 0;
	std::vector<Shape> m_Shapes;
	Yeastem::Renderer m_Renderer;
	std::unique_ptr<FrameBuffer> m_FrameBuffer;
};

YEASTEM_END

