#pragma once

#include "Scene.h"

#include <entt.hpp>

YEASTEM_BEGIN

class Entity
{
public:
	Entity(entt::entity entity, Scene* scene) :m_Entity(entity), m_Scene(scene) {}

	template <typename Component>
	bool HasComponent() const
	{
		return m_Scene->m_Registry.any_of<Component>(m_Entity);
	}

	template <typename Component, typename... Args>
	Component& AddComponent(Args... args)
	{
		YEASTEM_ASSERT("Entity already has Component!", !HasComponent<Component>());
		return m_Scene->m_Registry.emplace<Component>(m_Entity, std::forward<Args>(args)...);
	}

	template <typename Component>
	Component& GetComponent() const
	{
		YEASTEM_ASSERT("Entity does not have Component!", HasComponent<Component>());
		return m_Scene->m_Registry.get<Component>(m_Entity);
	}

	template <typename Component>
	void RemoveComponent()
	{
		YEASTEM_ASSERT("Entity does not have Component!", HasComponent<Component>());
		m_Scene->m_Registry.remove<Component>(m_Entity);
	}

	void Destroy()
	{
		m_Scene->m_Registry.destroy(m_Entity);
	}

	operator bool() const { return (uint32_t)m_Entity != 0U; }
	operator entt::entity&() { return m_Entity; }

private:
	entt::entity m_Entity{ 0 };
	Scene* m_Scene = nullptr;
};

YEASTEM_END
