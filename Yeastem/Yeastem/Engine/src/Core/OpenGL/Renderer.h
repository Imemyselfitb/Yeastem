#pragma once

#include "Core/OpenGL/Buffers/IndexBuffer.h"
#include "Core/OpenGL/Buffers/VertexBuffer.h"
#include "Core/OpenGL/Buffers/VertexArray.h"

#include "Core/OpenGL/Shader.h"
#include "Core/OpenGL/Texture.h"

#include "Core/Math/Vector2.h"

#include "Core/ResourceManager.h"

#include "Core/Scene/Components.h"

YEASTEM_BEGIN

struct QuadVertex
{
	Vector2 Position{};
	Vector2 Texture{};
	float TextureIndex{0};
};

/// <summary>
/// Batch for Rendering under one Draw Call
/// </summary>
template <typename VertexType, uint32_t VertexPerShape>
struct RenderBatch
{
	std::shared_ptr<VertexArray> LayoutPtr;
	std::shared_ptr<VertexBuffer> VertexBufferPtr;
	std::shared_ptr<IndexBuffer> IndexBufferPtr;

	ObjectID ShaderResource{ 0 };

	uint32_t IndexCount = 0;
	uint32_t VertexCount = 0;
	uint32_t TextureCount = 0;
	IndexBuffer::Type* IndicesArrayPtr = nullptr;

	VertexType* VerticesArrayPtr = nullptr;
	VertexType* VertexProgressPtr = nullptr;

	uint32_t MaxVertices = 0;
	uint32_t MaxIndices = 0;

	ObjectID BoundTextures[16]{ 0 };

	Vector2 DefaultVertexPositions[VertexPerShape];
	Vector2 DefaultTexturePositions[VertexPerShape];
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

	static void Submit(const RenderQuadComponent& renderable, const TransformComponent& transform, ResourceManager& resourceManager, float windowWidth, float windowHeight);

	static void SetQuadShader(ObjectID shaderID);
	static ObjectID GetQuadShaderID();
};

YEASTEM_END
