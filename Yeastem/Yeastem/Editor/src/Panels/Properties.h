#pragma once

#include "Yeastem/Core/Scene/Components.h"
#include "SceneHierarchy.h"

YEASTEM_BEGIN

namespace PropertyPanel
{
	void Render(bool& isOpen, HierarchyPanel& hierarchyPanel);

	template <typename Component> void RenderComponent(Entity& entity) {}
	template<> void RenderComponent<TransformComponent>(Entity& entity);
	template<> void RenderComponent<RenderQuadComponent>(Entity& entity);
	template<> void RenderComponent<ScriptComponent>(Entity& entity);
	template<> void RenderComponent<TagComponent>(Entity& entity);
};

YEASTEM_END
