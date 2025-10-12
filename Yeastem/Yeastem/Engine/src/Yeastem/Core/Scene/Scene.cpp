#include "yst_pch.h"

#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include "Yeastem/Core/Application.h"

#include "Yeastem/Core/Lua/LuaVector2.h"
#include "Yeastem/Core/Lua/LuaPanel.h"

YEASTEM_BEGIN

void Scene::Init(ObjectID sceneID, ResourceManager& resourceManager)
{
	m_SceneID = sceneID;
	Renderer::Init(resourceManager);
	m_LuaScene.Init();
}

Entity Scene::CreateEntity(const char* NodePath, const char* Name, entt::entity Parent)
{
	Entity entity{ m_Registry.create(), this };
	entity.template AddComponent<TagComponent>(NodePath, Name, Parent);
	return entity;
}

Entity Scene::GetRootEntity()
{
	return Entity(m_RootEntity, this);
}

template<typename T>
constexpr bool IsEnttGroupOwnable = \
	std::is_trivially_copyable_v<T> && \
	std::is_trivially_move_constructible_v<T> && \
	std::is_trivially_destructible_v<T>;

template<typename Component>
void Scene::SetScriptAllComponents()
{
	if constexpr (IsEnttGroupOwnable<Component>)
	{
		entt::basic_group group = m_Registry.template group<Component>(entt::template get<TagComponent>);
		for (entt::entity entity : group)
		{
			Component& component = group.template get<Component>(entity);
			TagComponent& tag = group.template get<TagComponent>(entity);

			m_LuaScene.SetComponent(component, tag);
		}
	}
	else
	{
		entt::basic_group group = m_Registry.template group<>(entt::get<Component, TagComponent>);
		for (entt::entity entity : group)
		{
			Component& component = m_Registry.template get<Component>(entity);
			TagComponent& tag = m_Registry.template get<TagComponent>(entity);

			m_LuaScene.SetComponent(component, tag);
		}
	}
}

template<typename Component>
void Scene::GetScriptAllComponents()
{
	entt::basic_group group = m_Registry.template group<>(entt::template get<Component, TagComponent>);
	for (entt::entity entity : group)
	{
		Component& component = m_Registry.template get<Component>(entity);
		TagComponent& tag = m_Registry.template get<TagComponent>(entity);

		m_LuaScene.GetComponent(component, tag);
	}
}

void Scene::InitScripts()
{
	m_ScriptsInitiated = true;
	entt::basic_group scriptGroup = m_Registry.template group<ScriptComponent>(entt::template get<TagComponent>);
	for (entt::entity entity : scriptGroup)
	{
		ScriptComponent& script = scriptGroup.template get<ScriptComponent>(entity);
		TagComponent& tag = scriptGroup.template get<TagComponent>(entity);

		m_LuaScene.ExecuteScript(script, tag);
	}

#if YST_CONFIG_DEBUG
	m_LuaScene.PrintGlobal("Yeastem");
#endif
}

void Scene::UpdateScriptObjects()
{
	SetScriptAllComponents<TransformComponent>();
	SetScriptAllComponents<RenderQuadComponent>();
}

void Scene::UpdateGlobalTransforms(entt::entity entity, TransformComponent& transform, uint32_t frameID)
{
	if (transform.FrameUpdate == frameID)
		return;

	TagComponent& tag = m_Registry.template get<TagComponent>(entity);
	
	if (tag.Parent == entt::null || 
		!m_Registry.valid(tag.Parent) || 
		!m_Registry.template any_of<TransformComponent>(tag.Parent))
	{
		transform.GlobalPosition = transform.Position;
		transform.GlobalRotation = transform.Rotation;
		transform.GlobalZLevel = transform.ZLevel;
		transform.GlobalScale = transform.Scale;
		transform.FrameUpdate = frameID;
		return;
	}
	
	TransformComponent& parentTransform = m_Registry.template get<TransformComponent>(tag.Parent);
	UpdateGlobalTransforms(tag.Parent, parentTransform, frameID);

	transform.GlobalPosition = parentTransform.TransformPoint(transform.Position);
	transform.GlobalRotation = transform.Rotation + parentTransform.Rotation;
	transform.GlobalZLevel = transform.ZLevel + parentTransform.ZLevel;
	transform.GlobalScale = transform.Scale * parentTransform.Scale;
	transform.FrameUpdate = frameID;
}

void Scene::Update(float deltaTime)
{
	if (SceneSize.x == 0 || SceneSize.y == 0)
		return;

	static uint32_t frameID = 0;
	frameID++;

	entt::basic_view transforms = m_Registry.template view<TransformComponent, TagComponent>();
	for (entt::entity entity : transforms)
	{
		TransformComponent& transform = m_Registry.template get<TransformComponent>(entity);
		UpdateGlobalTransforms(entity, transform, frameID);
	}
	
	if (IsRunning)
	{
		SetScriptAllComponents<TransformComponent>();
		SetScriptAllComponents<RenderQuadComponent>();

		if (!m_ScriptsInitiated)
			InitScripts();

		m_LuaScene.UpdateWindow(SceneSize);
		
		entt::basic_group scriptGroup = m_Registry.template group<ScriptComponent>(entt::template get<TagComponent>);
		for (entt::entity entity : scriptGroup)
		{
			TagComponent& tag = m_Registry.template get<TagComponent>(entity);
			m_LuaScene.CallYeastemFunction(tag, "Update", deltaTime);
		}

		GetScriptAllComponents<TransformComponent>();
		CurrentTime += uint64_t(deltaTime * 1000.0f);
	}
}

void Scene::RecreateFrameBuffer(uint32_t width, uint32_t height)
{
	if (!m_FrameBuffer)
		return CreateFrameBuffer(width, height);

	m_FrameBuffer->Destroy();
	m_FrameBuffer->Create(width, height);
}

void Scene::CreateFrameBuffer(uint32_t width, uint32_t height)
{
	m_FrameBuffer = std::template make_unique<FrameBuffer>(width, height);
}

void Scene::Render(ResourceManager& resourceManager)
{
	if (SceneSize.x == 0 || SceneSize.y == 0)
		return;

	if (m_FrameBuffer)
		m_FrameBuffer->Bind();

	glViewport(0, 0, (uint32_t)SceneSize.x, (uint32_t)SceneSize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Renderer::BeginScene(resourceManager);

	entt::basic_group quadRenderables = m_Registry.template group<RenderQuadComponent, TransformComponent>();
	quadRenderables.template sort<TransformComponent>([](const TransformComponent& a, const TransformComponent& b) {
		return a.GlobalZLevel < b.GlobalZLevel;
	});
	for (entt::entity entity : quadRenderables)
	{
		const auto& [quad, transform] = quadRenderables.template get<RenderQuadComponent, TransformComponent>(entity);
		Renderer::Submit(quad, transform, resourceManager, SceneSize);
	}

	Renderer::EndScene(resourceManager);

	if (m_FrameBuffer)
		m_FrameBuffer->Unbind();

	LuaImguiPanel::ShowAll(m_LuaScene.GetState());
}


YEASTEM_END
