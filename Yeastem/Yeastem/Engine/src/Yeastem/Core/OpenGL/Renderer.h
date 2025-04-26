#pragma once

#include "Shader.h"
#include "Texture.h"

#include "Yeastem/Core/Math/Vector2.h"
#include "Yeastem/Core/ResourceManager.h"
#include "Yeastem/Core/Scene/Components.h"

YEASTEM_BEGIN

struct QuadVertex
{
	// Psuedo-3D position
	Vector2f Position{};
	float ZLevel{ 0 };

	Vector2f Texture{};
	float TextureIndex{ 0 };
};

template<typename VertexType>
class Shape;

/// <summary>
/// Renderer which combines data into a single Draw Call!
/// </summary>
class Renderer
{
public:
	Renderer() {}

public:
	static void Init(ResourceManager& resourceManager);

	static void BeginScene(ResourceManager& resourceManager);
	static void EndScene(ResourceManager& resourceManager);

	static uint32_t BindQuadTexture(Texture& curTexture, ObjectID curTextureID);
	static void Submit(
		const RenderQuadComponent& renderable, const TransformComponent& transform, 
		ResourceManager& resourceManager, Vector2f renderArea
	);

	static void OnWindowResize();

	static void SetQuadShader(ObjectID shaderID);
	static ObjectID GetQuadShaderID();
};

YEASTEM_END
