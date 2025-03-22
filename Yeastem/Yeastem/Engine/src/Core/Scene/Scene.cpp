#include "yst_pch.h"

#include "Scene.h"

#include "Entity.h"
#include "Components.h"

#include "Core/Application.h"

#include "Core/Lua/LuaVector2.h"
#include "Core/Lua/LuaPanel.h"

YEASTEM_BEGIN

void Scene::Init(ResourceManager& resourceManager)
{
	Renderer::Init(resourceManager);
	m_LuaScene.Init();
}

Entity Scene::CreateEntity(const char* NodePath, const char* Name)
{
	Entity entity{ m_Registry.create(), this };
	entity.AddComponent<TagComponent>(NodePath, Name);
	return entity;
}

void Scene::ProcessEvent(SDL_Event& evt)
{
	if (evt.type == SDL_KEYDOWN && !evt.key.repeat)
	{
		if (evt.key.keysym.sym == SDLK_h)
		{
			std::cout << "PLAY/PAUSE [H]\n";
			IsRunning = !IsRunning;
			IsReset = false;
		}
		else if (evt.key.keysym.sym == SDLK_j)
		{
			std::cout << "RESET [J]\n";
			IsRunning = false;
			IsReset = true;
		}
	}

	m_EventHandler.Dispatch(evt);
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
		entt::basic_group group = m_Registry.group<Component>(entt::get<TagComponent>);
		for (entt::entity entity : group)
		{
			Component& component = group.get<Component>(entity);
			TagComponent& tag = group.get<TagComponent>(entity);

			m_LuaScene.SetComponent(component, tag);
		}
	}
	else
	{
		entt::basic_group group = m_Registry.group<>(entt::get<Component, TagComponent>);
		for (entt::entity entity : group)
		{
			Component& component = m_Registry.get<Component>(entity);
			TagComponent& tag = m_Registry.get<TagComponent>(entity);

			m_LuaScene.SetComponent(component, tag);
		}
	}
}

template<typename Component>
void Scene::GetScriptAllComponents()
{
	entt::basic_group group = m_Registry.group<>(entt::get<Component, TagComponent>);
	for (entt::entity entity : group)
	{
		Component& component = m_Registry.get<Component>(entity);
		TagComponent& tag = m_Registry.get<TagComponent>(entity);

		m_LuaScene.GetComponent(component, tag);
	}
}

void Scene::InitScripts()
{
	m_ScriptsInitiated = true;
	entt::basic_group scriptGroup = m_Registry.group<ScriptComponent>(entt::get<TagComponent>);
	for (entt::entity entity : scriptGroup)
	{
		ScriptComponent& script = m_Registry.get<ScriptComponent>(entity);
		TagComponent& tag = m_Registry.get<TagComponent>(entity);

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

void Scene::Update(float deltaTime)
{
	if (SceneSize.x == 0 || SceneSize.y == 0)
		return;

	if (IsRunning)
	{
		SetScriptAllComponents<TransformComponent>();
		SetScriptAllComponents<RenderQuadComponent>();

		if (!m_ScriptsInitiated)
			InitScripts();

		m_LuaScene.UpdateWindow(SceneSize);
		
		entt::basic_group scriptGroup = m_Registry.group<ScriptComponent>(entt::get<TagComponent>);
		for (entt::entity entity : scriptGroup)
		{
			TagComponent& tag = m_Registry.get<TagComponent>(entity);
			m_LuaScene.CallYeastemFunction(tag, "Update", deltaTime);
		}

		GetScriptAllComponents<TransformComponent>();

		LuaImguiPanel::ShowAll(m_LuaScene.GetState());
		CurrentTime += uint64_t(deltaTime * 1000.0f);
	}
	else if (IsReset)
	{
		m_ScriptsInitiated = false;
		m_LuaScene.Recreate();
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
	m_FrameBuffer = std::make_unique<FrameBuffer>(width, height);
}

void Scene::Render(ResourceManager& resourceManager)
{
	if (SceneSize.x == 0 || SceneSize.y == 0)
		return;

	if (m_FrameBuffer)
	{
		m_FrameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Renderer::BeginScene(resourceManager);

	entt::basic_group quadRenderables = m_Registry.group<RenderQuadComponent>(entt::get<TransformComponent>);
	for (entt::entity entity : quadRenderables)
	{
		const auto& [quad, transform] = m_Registry.get<RenderQuadComponent, TransformComponent>(entity);
		Renderer::Submit(quad, transform, resourceManager, SceneSize.x, SceneSize.y);
	}

	Renderer::EndScene(resourceManager);

	if (m_FrameBuffer)
		m_FrameBuffer->Unbind();
}


YEASTEM_END
