#pragma once

#include "Yeastem/Core/OpenGL/Include.h"
#include "Profiling.h"
#include "Events.h"

#include "Yeastem/LuaAPI/Lua.h"
#include "LuaScene.h"

#include <entt.hpp>

YEASTEM_BEGIN

class Entity;

class Scene
{
public:
	Scene() {}
	Scene(const std::filesystem::path& scenePath) {}

public:
	void Init(ObjectID sceneID, ResourceManager& resourceManager);
	void Update(float DeltaTime);
	void Render(ResourceManager& resourceManager);
	ObjectID GetID() const { return m_SceneID; }

	Entity CreateEntity(const char* NodePath = "", const char* Name = "", entt::entity Parent = entt::null);
	entt::registry& GetRegistry() { return m_Registry; }

	Entity GetRootEntity();
	void SetRootEntity(entt::entity entity) { m_RootEntity = entity; }

	void CreateFrameBuffer(uint32_t width, uint32_t height);
	void RecreateFrameBuffer(uint32_t width, uint32_t height);

	bool DoesFrameBufferExist() const { return m_FrameBuffer != nullptr; }
	uint32_t GetFrameBufferColourAttachmentID() const { return m_FrameBuffer->GetColourAttachmentID(); }
	uint32_t GetFrameBufferDepthAttachmentID() const { return m_FrameBuffer->GetDepthAttachmentID(); }

	void ResetScripts() { m_ScriptsInitiated = false; m_LuaScene.Recreate(); }

public:
	bool IsRunning = false;

	uint64_t CurrentTime = 0;
	Vector2i SceneSize{};

private:
	void InitScripts();
	void UpdateScriptObjects();
	void UpdateGlobalTransforms(entt::entity entity, TransformComponent& transform, uint32_t frameID);

	template<typename Component>
	void UpdateComponentsFromScript();
	template<typename Component>
	void SetComponentsToScript();

private:
	LuaScene m_LuaScene;

	std::unique_ptr<FrameBuffer> m_FrameBuffer;

	entt::registry m_Registry;
	entt::entity m_RootEntity = entt::null;

	bool m_ScriptsInitiated = false;

	ObjectID m_SceneID = 0;

private:
	friend class Entity;
	friend class LuaScene;
};

YEASTEM_END
