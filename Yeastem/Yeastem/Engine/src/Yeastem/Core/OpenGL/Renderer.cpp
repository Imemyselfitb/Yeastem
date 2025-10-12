#include "yst_pch.h"

#include "Renderer.h"

#include "Yeastem/FileIO/FileIO.h"

#include "Buffers/IndexBuffer.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/VertexArray.h"

YEASTEM_BEGIN

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

	Vector2f DefaultVertexPositions[VertexPerShape];
	Vector2f DefaultTexturePositions[VertexPerShape];
};

static RenderBatch<QuadVertex, 4> s_QuadData;

void Renderer::Init(ResourceManager& resourceManager)
{
	VertexBufferLayout layout;
	layout.template Push<float>(3);
	layout.template Push<float>(2);
	layout.template Push<float>(1);

	s_QuadData.MaxVertices = 400;
	s_QuadData.MaxIndices = 600;

	s_QuadData.VerticesArrayPtr = new QuadVertex[s_QuadData.MaxVertices];
	s_QuadData.VertexProgressPtr = s_QuadData.VerticesArrayPtr;

	s_QuadData.IndicesArrayPtr = new IndexBuffer::Type[s_QuadData.MaxIndices];

	uint32_t indexOffset = 0;
	for (uint32_t i = 0; i < s_QuadData.MaxIndices; i += 6)
	{
		s_QuadData.IndicesArrayPtr[i + 0] = indexOffset + 0;
		s_QuadData.IndicesArrayPtr[i + 1] = indexOffset + 1;
		s_QuadData.IndicesArrayPtr[i + 2] = indexOffset + 2;

		s_QuadData.IndicesArrayPtr[i + 3] = indexOffset + 2;
		s_QuadData.IndicesArrayPtr[i + 4] = indexOffset + 3;
		s_QuadData.IndicesArrayPtr[i + 5] = indexOffset + 0;

		indexOffset += 4;
	}

	s_QuadData.DefaultVertexPositions[0] = { -0.5f, -0.5f, };
	s_QuadData.DefaultVertexPositions[1] = {  0.5f, -0.5f, };
	s_QuadData.DefaultVertexPositions[2] = {  0.5f,  0.5f, };
	s_QuadData.DefaultVertexPositions[3] = { -0.5f,  0.5f, };

	s_QuadData.DefaultTexturePositions[0] = { 0.0f, 0.0f };
	s_QuadData.DefaultTexturePositions[1] = { 1.0f, 0.0f };
	s_QuadData.DefaultTexturePositions[2] = { 1.0f, 1.0f };
	s_QuadData.DefaultTexturePositions[3] = { 0.0f, 1.0f };

	s_QuadData.VertexBufferPtr = std::template make_unique<VertexBuffer>(nullptr, (uint32_t)(s_QuadData.MaxVertices * sizeof(QuadVertex)));
	s_QuadData.IndexBufferPtr = std::template make_unique<IndexBuffer>(nullptr, s_QuadData.MaxIndices);

	s_QuadData.LayoutPtr = std::template make_shared<VertexArray>();
	s_QuadData.LayoutPtr->AddBuffer(*s_QuadData.VertexBufferPtr, layout);
}

void Renderer::SetQuadShader(ObjectID shaderID)
{
	s_QuadData.ShaderResource = shaderID;
}

ObjectID Renderer::GetQuadShaderID()
{
	return s_QuadData.ShaderResource;
}

void Renderer::BeginScene(ResourceManager& resourceManager)
{
	s_QuadData.VertexProgressPtr = s_QuadData.VerticesArrayPtr;
	s_QuadData.IndexCount = 0;
	s_QuadData.VertexCount = 0;
	s_QuadData.TextureCount = 0;

	int textureUnits[16];
	for (int i = 0; i < 16; ++i)
		textureUnits[i] = i;
	resourceManager.Shaders.Get(s_QuadData.ShaderResource).SetUniform1iv("u_textures", 16, textureUnits);
}

uint32_t Renderer::BindQuadTexture(Texture& curTexture, ObjectID curTextureID)
{
	if (!curTexture.isBound() || s_QuadData.BoundTextures[curTexture.getBoundSlot()] != curTextureID)
	{
		curTexture.Bind(s_QuadData.TextureCount);
		s_QuadData.BoundTextures[curTexture.getBoundSlot()] = curTextureID;
		s_QuadData.TextureCount++;
	}
	return curTexture.getBoundSlot();
}

void Renderer::Submit(
	const RenderQuadComponent& renderable, const TransformComponent& transform,
	ResourceManager& resourceManager, Vector2f renderArea)
{
	if (s_QuadData.VertexCount >= 400 || s_QuadData.TextureCount >= 16)
	{
		EndScene(resourceManager);
		BeginScene(resourceManager);
	}

	uint32_t boundTextureID = 0;
	if (renderable.Textures.size() > 0)
	{
		uint32_t currentTexture = SDL_min(renderable.CurrentTexture, (uint32_t)renderable.Textures.size()-1);
		ObjectID curTextureID = renderable.Textures[currentTexture];
		Texture& curTexture = resourceManager.Textures.Get(curTextureID);
		boundTextureID = BindQuadTexture(curTexture, curTextureID);
	}

	// Add Vertices
	for (int i = 0; i < 4; i++)
	{
		Vector2f pos = transform.TransformPoint(s_QuadData.DefaultVertexPositions[i] * renderable.Size);
		pos /= renderArea * Vector2f(0.5f, 0.5f);
		pos -= Vector2f(1.0f, 1.0f);
		s_QuadData.VertexProgressPtr->Position = pos;
		s_QuadData.VertexProgressPtr->ZLevel = 0.0f - transform.ZLevel;
		s_QuadData.VertexProgressPtr->Texture = s_QuadData.DefaultTexturePositions[i];
		s_QuadData.VertexProgressPtr->TextureIndex = (float)boundTextureID;
		s_QuadData.VertexProgressPtr++;
		s_QuadData.VertexCount++;
	}

	s_QuadData.IndexCount += 6;
}

void Renderer::OnWindowResize()
{
	for (uint32_t i = 0; i < 16; i++)
		s_QuadData.BoundTextures[i] = 0;
}

void Renderer::EndScene(ResourceManager& resourceManager)
{
	s_QuadData.VertexBufferPtr->UpdateBuffer(s_QuadData.VerticesArrayPtr);
	s_QuadData.IndexBufferPtr->UpdateBuffer(s_QuadData.IndicesArrayPtr);
	s_QuadData.VertexBufferPtr->Bind();
	s_QuadData.IndexBufferPtr->Bind();
	s_QuadData.LayoutPtr->Bind();
	resourceManager.Shaders.Get(s_QuadData.ShaderResource).Bind();
	
	glDrawElements(GL_TRIANGLES, s_QuadData.IndexCount, GL_UNSIGNED_INT, nullptr);
}

YEASTEM_END
