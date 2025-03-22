#pragma once

#include "Core/OpenGL/Include.h"
#include "Core/Scene/Events.h"
#include "LuaAPI/Lua.h"
#include "Core/Scene/Profiling.h"

#include "LuaScene.h"

#include <entt.hpp>

YEASTEM_BEGIN

class Entity;

class Scene
{
public:
	Scene() {}

public:
	void Init(ResourceManager& resourceManager);
	void Update(float DeltaTime);
	void Render(ResourceManager& resourceManager);
	void ProcessEvent(SDL_Event& evt);

	Entity CreateEntity(const char* NodePath = "", const char* Name = "");

	void CreateFrameBuffer(uint32_t width, uint32_t height);
	void RecreateFrameBuffer(uint32_t width, uint32_t height);

	bool DoesFrameBufferExist() const { return m_FrameBuffer != nullptr; }
	uint32_t GetFrameBufferColourAttachmentID() const { return m_FrameBuffer->GetColourAttachmentID(); }
	uint32_t GetFrameBufferDepthAttachmentID() const { return m_FrameBuffer->GetDepthAttachmentID(); }

public:
	bool IsRunning = false;
	bool IsReset = true;
	bool ShouldRefreshFrameBuffer = false;

	uint64_t CurrentTime = 0;
	ImVec2 SceneSize;

private:
	void InitScripts();
	void UpdateScriptObjects();

	template<typename Component>
	void SetScriptAllComponents();
	template<typename Component>
	void GetScriptAllComponents();

private:
	Yeastem::EventHandler m_EventHandler;
	LuaScene m_LuaScene;

	std::unique_ptr<FrameBuffer> m_FrameBuffer;

	entt::registry m_Registry;

	bool m_ScriptsInitiated = false;

private:
	friend class Entity;
	friend class LuaScene;
};

YEASTEM_END
