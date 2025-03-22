#include "yst_pch.h"

#include "Renderer.h"

#include "FileIO/FileIO.h"

YEASTEM_BEGIN

static RenderBatch<QuadVertex, 4> s_QuadData;

void Renderer::Init(ResourceManager& resourceManager)
{
	VertexBufferLayout layout;
	layout.Push<float>(2);
	layout.Push<float>(2);
	layout.Push<float>(1);

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

	s_QuadData.VertexBufferPtr = std::make_unique<VertexBuffer>(nullptr, static_cast<uint32_t>(s_QuadData.MaxVertices * sizeof(QuadVertex)));
	s_QuadData.IndexBufferPtr = std::make_unique<IndexBuffer>(nullptr, s_QuadData.MaxIndices);

	s_QuadData.LayoutPtr = std::make_shared<VertexArray>();
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

void Renderer::Submit(
	const RenderQuadComponent& renderable, const TransformComponent& transform,
	ResourceManager& resourceManager, float windowWidth, float windowHeight)
{
	if (s_QuadData.VertexCount >= 400 || s_QuadData.TextureCount >= 16)
	{
		EndScene(resourceManager);
		BeginScene(resourceManager);
	}

	ObjectID curTextureID = renderable.Textures[renderable.CurrentTexture];
	Texture& curTexture = resourceManager.Textures.Get(curTextureID);

	if (!curTexture.isBound() || s_QuadData.BoundTextures[curTexture.getBoundSlot()] != curTextureID)
	{
		curTexture.Bind(s_QuadData.TextureCount);
		s_QuadData.BoundTextures[curTexture.getBoundSlot()] = curTextureID;
		s_QuadData.TextureCount++;
		std::cout << "Texture " << curTextureID << " is Bound to Slot: " << curTexture.getBoundSlot() << "\n";
	}

	// Add Vertices
	for (int i = 0; i < 4; i++)
	{
		Vector2 pos = transform.TransformPoint(s_QuadData.DefaultVertexPositions[i] * renderable.Size);
		pos /= Vector2(windowWidth * 0.5f, windowHeight * -0.5f);
		pos += Vector2(-1.0f, 1.0f);
		s_QuadData.VertexProgressPtr->Position = pos;
		s_QuadData.VertexProgressPtr->Texture = s_QuadData.DefaultTexturePositions[i];
		s_QuadData.VertexProgressPtr->TextureIndex = static_cast<float>(curTexture.getBoundSlot());
		s_QuadData.VertexProgressPtr++;
		s_QuadData.VertexCount++;
	}

	s_QuadData.IndexCount += 6;
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
