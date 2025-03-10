#include "yst_pch.h"

#include "RendererAPI.h"

#include "FileIO/FileIO.h"

#include "Shape.h"

YEASTEM_BEGIN

template struct Renderable<QuadVertex>;

static RenderBatch s_QuadData;

void BatchRenderer::Init(ResourceManager& resourceManager)
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

	s_QuadData.VertexBufferPtr = std::make_unique<VertexBuffer>(nullptr, static_cast<uint32_t>(s_QuadData.MaxVertices * sizeof(QuadVertex)));
	s_QuadData.IndexBufferPtr = std::make_unique<IndexBuffer>(nullptr, s_QuadData.MaxIndices);

	s_QuadData.LayoutPtr = std::make_shared<VertexArray>();
	s_QuadData.LayoutPtr->AddBuffer(*s_QuadData.VertexBufferPtr, layout);
}

void BatchRenderer::SetQuadShader(ObjectID shaderID)
{
	s_QuadData.ShaderResource = shaderID;
}

void BatchRenderer::BeginScene(ResourceManager& resourceManager) const
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

ObjectID BatchRenderer::GetQuadShaderID()
{
	return s_QuadData.ShaderResource;
}

void BatchRenderer::Submit(Shape<QuadVertex>& quad, ResourceManager& resourceManager, float windowWidth, float windowHeight) const
{
	Renderable<QuadVertex>& quadRenderable = quad.GetRenderable();

	if (s_QuadData.VertexCount >= 400 || s_QuadData.TextureCount >= 16)
	{
		EndScene(resourceManager);
		BeginScene(resourceManager);
	}

	ObjectID curTextureID = quadRenderable.Textures[quadRenderable.CurrentTexture];
	Texture& curTexture = resourceManager.Textures.Get(curTextureID);

	if (!curTexture.isBound() || s_QuadData.BoundTextures[curTexture.getBoundSlot()] != curTextureID)
	{
		curTexture.Bind(s_QuadData.TextureCount);
		s_QuadData.BoundTextures[curTexture.getBoundSlot()] = curTextureID;
		s_QuadData.TextureCount++;
		std::cout << "Texture " << curTextureID << " is Bound to Slot: " << curTexture.getBoundSlot() << "\n";
	}

	// Add Vertices
	for (int i = 0; i < quadRenderable.VertexList.size(); i++)
	{
		*s_QuadData.VertexProgressPtr = quadRenderable.VertexList[i];
		s_QuadData.VertexProgressPtr->Position = quad.GetVertexPosition(i, windowWidth, windowHeight);
		s_QuadData.VertexProgressPtr->TextureIndex = static_cast<float>(curTexture.getBoundSlot());
		s_QuadData.VertexProgressPtr++;
		s_QuadData.VertexCount++;
	}

	//// Generic - Add Indices (Not used for Sprites)
	//for (const IndexBuffer::Type& index : quadRenderable.IndexList)
	//{
	//	*s_QuadData.IndexProgressPtr = index + s_QuadData.VertexCount;
	//	s_QuadData.IndexProgressPtr++;
	//	s_QuadData.IndexCount++;
	//}

	s_QuadData.IndexCount += 6;
}

void BatchRenderer::EndScene(ResourceManager& resourceManager) const
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
