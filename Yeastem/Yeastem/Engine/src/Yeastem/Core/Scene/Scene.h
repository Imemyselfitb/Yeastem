#pragma once

#include "Yeastem/Core/OpenGL/Include.h"
#include "Yeastem/LuaAPI/Lua.h"
#include "Profiling.h"
#include "Events.h"

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

	Entity CreateEntity(const char* NodePath = "", const char* Name = "", entt::entity Parent = entt::null);

	void CreateFrameBuffer(uint32_t width, uint32_t height);
	void RecreateFrameBuffer(uint32_t width, uint32_t height);

	bool DoesFrameBufferExist() const { return m_FrameBuffer != nullptr; }
	uint32_t GetFrameBufferColourAttachmentID() const { return m_FrameBuffer->GetColourAttachmentID(); }
	uint32_t GetFrameBufferDepthAttachmentID() const { return m_FrameBuffer->GetDepthAttachmentID(); }

	EventHandler& GetEventHandler() { return m_EventHandler; }
	const EventHandler& GetEventHandler() const { return m_EventHandler; }
	
public:
	bool IsRunning = false;
	bool IsReset = false;

	uint64_t CurrentTime = 0;
	Vector2i SceneSize;

private:
	void InitScripts();
	void UpdateScriptObjects();
	void UpdateGlobalTransforms(entt::entity entity, TransformComponent& transform, uint32_t frameID);

	template<typename Component>
	void SetScriptAllComponents();
	template<typename Component>
	void GetScriptAllComponents();

private:
	EventHandler m_EventHandler;
	LuaScene m_LuaScene;

	std::unique_ptr<FrameBuffer> m_FrameBuffer;

	entt::registry m_Registry;

	bool m_ScriptsInitiated = false;

private:
	friend class Entity;
	friend class LuaScene;
};

YEASTEM_END
