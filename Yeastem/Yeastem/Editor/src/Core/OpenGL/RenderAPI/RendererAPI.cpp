#include "yst_pch.h"

#include "RendererAPI.h"
#include "Core/OpenGL/ShaderContainer.h"

YEASTEM_BEGIN

void Renderable::AssignVertexBuffer(const void* data, unsigned int size)
{
	this->vertices = std::make_unique<VertexBuffer>(data, size);
}
void Renderable::AssignVertices(const std::vector<Vertex>& vertexList)
{
	this->VertexList = vertexList;
	this->vertices = std::make_unique<VertexBuffer>(&vertexList[0], vertexList.size() * sizeof(Vertex));
	return;
}
void Renderable::AssignIndexBuffer(const unsigned int* data, unsigned int count)
{
	this->indices = std::make_unique<IndexBuffer>(data, count);
}
void Renderable::AddVertexBufferLayout(const VertexBufferLayout& layout)
{
	if (this->layout == nullptr) 
		this->layout = std::make_unique<VertexArray>();

	this->layout->AddBuffer(*this->vertices, layout);
}
void Renderable::AssignShaderFromFiles(const std::string& VertexShader, const std::string& FragmentShader)
{
	std::shared_ptr<Shader> shader = ShaderContainer::GetShader({ VertexShader, FragmentShader });
	if (shader)
	{
		this->shader = shader;
		return;
	}

	if(this->shader)
		this->shader->AssignShaderFromFiles(VertexShader, FragmentShader);
	else
		this->shader = std::make_shared<Shader>(VertexShader, FragmentShader);

	ShaderContainer::AddShader({ VertexShader, FragmentShader }, this->shader);
}

void Renderable::ReserveTextures(unsigned int count)
{
	this->textures.reserve(count);
}
void Renderable::AddTexture(const std::string& path)
{
	this->textures.emplace_back(path);
}

void Renderable::ReloadIndices()
{
	//this->indices->UpdateBuffer(&(*this->IndexList)[0], this->IndexList->size());
}

void Renderable::ReloadVertices()
{
	this->vertices->UpdateBuffer(&this->VertexList[0]);
}

void BasicRenderer::Render(const Renderable& obj)
{
	if (obj.textures.size() > 32)
	{
		YEASTEM_WARNING("ONLY BOUND 32 TEXTURES OF OBJECT - " << obj.textures.size() << " EXISTED")
	}

	if (!obj.indices || !obj.layout || !obj.vertices || !obj.shader)
	{
		YEASTEM_ERROR("COULD NOT RENDER OBJECTS :- Doing so would lead to a Segmentation Fault!s")
		return;
	}
	
	obj.indices->Bind();
	obj.layout->Bind();
	obj.vertices->Bind();
	obj.shader->Bind();

	const size_t TextureBindCount = SDL_min(obj.textures.size(), 32);
	for (int i = 0; i < TextureBindCount; i++)
	{
		obj.textures[i].Bind(i);
		glUniform1i(glGetUniformLocation(*obj.shader, ("u_tex" + std::to_string(i)).c_str()), i);
	}

	glDrawElements(GL_TRIANGLES, obj.indices->size, GL_UNSIGNED_INT, nullptr);

	obj.indices->Unbind();
	obj.vertices->Unbind();
	obj.shader->Unbind();
}

YEASTEM_END
