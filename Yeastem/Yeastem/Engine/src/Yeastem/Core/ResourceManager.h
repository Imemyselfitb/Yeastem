#pragma once

#include "OpenGL/Shader.h"
#include "OpenGL/Texture.h"

YEASTEM_BEGIN

template <typename ResourceType>
class SingleResourceManager
{
public:
	SingleResourceManager() {}

public:
	template <typename... Args>
	ObjectID Load(Args... args)
	{
		ObjectID objID = 0;
		(..., hashCombine(objID, std::forward<Args>(args)));
		if (!Exists(objID))
			m_Resources.emplace(std::piecewise_construct, std::forward_as_tuple(objID), std::forward_as_tuple(std::forward<Args>(args)...));

		return objID;
	}

	void Unload(ObjectID objID) { m_Resources.erase(objID); }

	std::unordered_map<ObjectID, ResourceType>& GetResources() { return m_Resources; }
	ResourceType& Get(ObjectID objID) { return m_Resources.at(objID); }

	bool Exists(ObjectID objID) { return m_Resources.find(objID) != m_Resources.end(); }
	void Clear() { m_Resources.clear(); }

private:
	template <typename T>
	static void hashCombine(std::size_t& seed, const T& value) {
		seed ^= (std::hash<T>()(value)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

private:
	std::unordered_map<ObjectID, ResourceType> m_Resources;
};

struct ResourceManager
{
	SingleResourceManager<Texture> Textures;
	SingleResourceManager<Shader> Shaders;

	ResourceManager() {}
	ResourceManager(const ResourceManager&) = delete;
};

YEASTEM_END
