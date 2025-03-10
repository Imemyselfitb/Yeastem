#pragma once

#include "Core/OpenGL/Buffers/IndexBuffer.h"
#include "Core/OpenGL/Buffers/VertexBuffer.h"
#include "Core/OpenGL/Buffers/VertexArray.h"

#include "Core/OpenGL/Shader.h"
#include "Core/OpenGL/Texture.h"

#include "Core/Math/Vector2.h"

#include "Core/ResourceManager.h"

YEASTEM_BEGIN

struct QuadVertex
{
	Vector2 Position{};
	Vector2 Texture{};
	float TextureIndex{0};
};

/// <summary>
/// Renderable (used for Shape, Lines, etc.)
/// </summary>
/// <typeparam name="VertexType"></typeparam>
template<typename VertexType>
struct Renderable
{
	uint32_t CurrentTexture = 0;

	std::vector<ObjectID> Textures{};
	std::vector<VertexType> VertexList{};
	std::vector<IndexBuffer::Type> IndexList{};
};

/// <summary>
/// Batch for Rendering under one Draw Call
/// </summary>
struct RenderBatch
{
	std::shared_ptr<VertexArray> LayoutPtr;
	std::shared_ptr<VertexBuffer> VertexBufferPtr;
	std::shared_ptr<IndexBuffer> IndexBufferPtr;

	ObjectID ShaderResource;

	uint32_t IndexCount = 0;
	uint32_t VertexCount = 0;
	uint32_t TextureCount = 0;
	IndexBuffer::Type* IndicesArrayPtr = nullptr;

	QuadVertex* VerticesArrayPtr = nullptr;
	QuadVertex* VertexProgressPtr = nullptr;

	uint32_t MaxVertices = 0;
	uint32_t MaxIndices = 0;

	ObjectID BoundTextures[16];
};

template<typename VertexType>
class Shape;

/// <summary>
/// Renderer which combines data into a single Draw Call!
/// </summary>
class BatchRenderer
{
public:
	BatchRenderer() {}

public:
	void Init(ResourceManager& resourceManager);
	void SetQuadShader(ObjectID shaderID);

	void BeginScene(ResourceManager& resourceManager) const;

	void Submit(Shape<QuadVertex>& renderable, ResourceManager& resourceManager, float windowWidth, float windowHeight) const;

	void EndScene(ResourceManager& resourceManager) const;

	ObjectID GetQuadShaderID();
};

YEASTEM_END
