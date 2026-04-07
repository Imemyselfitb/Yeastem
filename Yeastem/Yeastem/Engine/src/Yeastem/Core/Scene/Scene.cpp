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
void Scene::SetComponentsToScript()
{
	m_Registry.template view<Component, TagComponent>().each(
		[this](const Component& component, const TagComponent& tag) -> void {
			m_LuaScene.SetComponentToScript(component, tag);
		}
	);
}

template<typename Component>
void Scene::UpdateComponentsFromScript()
{
	m_Registry.template view<Component, TagComponent>().each(
		[this](Component& component, const TagComponent& tag) -> void {
			m_LuaScene.UpdateComponentFromScript(component, tag);
		}
	);
}

void Scene::InitScripts()
{
	m_ScriptsInitiated = true;
	entt::basic_view scriptGroup = m_Registry.template view<ScriptComponent, TagComponent>();
	scriptGroup.each([this](const ScriptComponent& script, const TagComponent& tag) -> void {
		m_LuaScene.ExecuteScript(script, tag);
	});

#if YST_CONFIG_DEBUG
	m_LuaScene.PrintGlobal("Yeastem");
#endif
}

void Scene::UpdateScriptObjects()
{
	SetComponentsToScript<TransformComponent>();
	SetComponentsToScript<RenderQuadComponent>();
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
		SetComponentsToScript<TransformComponent>();
		SetComponentsToScript<RenderQuadComponent>();

		if (!m_ScriptsInitiated)
			InitScripts();

		m_LuaScene.UpdateWindow(SceneSize);
		
		m_Registry.template group<ScriptComponent>(entt::template get<TagComponent>).each(
			[this, deltaTime](
				const ScriptComponent& script, const TagComponent& tag
			) -> void {
				m_LuaScene.CallYeastemFunction(tag, "Update", deltaTime);
			}
		);

		UpdateComponentsFromScript<TransformComponent>();
		UpdateComponentsFromScript<RenderQuadComponent>();
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

	Renderer::BeginScene(resourceManager, SceneSize);

	entt::basic_group quadRenderables = m_Registry.template group<RenderQuadComponent, TransformComponent>();
	quadRenderables.template sort<TransformComponent>([](const TransformComponent& a, const TransformComponent& b) {
		return a.GlobalZLevel < b.GlobalZLevel;
	});
	quadRenderables.each(
		[&resourceManager, this](
			const RenderQuadComponent& quad, const TransformComponent& transform
		) -> void {
			Renderer::Submit(quad, transform, resourceManager, SceneSize);
		}
	);

	Renderer::EndScene(resourceManager);

	if (m_FrameBuffer)
		m_FrameBuffer->Unbind();

	LuaImguiPanel::ShowAll(m_LuaScene.GetState());
}


YEASTEM_END
