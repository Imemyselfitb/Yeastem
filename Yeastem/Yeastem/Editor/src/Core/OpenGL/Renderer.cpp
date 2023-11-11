#include "yst_pch.h"

#include "Renderer.h"

YEASTEM_BEGIN

#define DestoryUniquePtr(ptr) ptr.~unique_ptr();

void Renderer::AssignIndexBuffer(const unsigned int* data, unsigned int count)
{
	DestoryUniquePtr(this->m_IndexBuffer);
	this->m_IndexBuffer = std::make_unique<IndexBuffer>(data, count);
}

void Renderer::AssignVertexBuffer(const void* data, unsigned int size)
{
	DestoryUniquePtr(this->m_VertexBuffer);
	this->m_VertexBuffer = std::make_unique<VertexBuffer>(data, size);
}

void Renderer::AddVertexBufferLayout(const VertexBufferLayout& layout)
{
	if (this->m_VertexArray == NULL)
		this->m_VertexArray = std::make_unique<VertexArray>();
	this->m_VertexArray->AddBuffer(*m_VertexBuffer, layout);
}

void Renderer::AssignShader(const Shader& other)
{
	this->m_Shader = other;
}

void Renderer::AssignShaderFromFiles(const std::string& vert, const std::string& frag)
{
	this->m_Shader.AssignShaderFromFiles(vert, frag);
}

void Renderer::AddTexture(const std::string& path)
{
	for (Texture& texture : this->m_Texture)
	{
		if (!texture.isInitialised())
		{
			texture.LoadTexture(path);
			return;
		}
	}

	this->m_Texture.emplace_back(path);
}

void Renderer::ReserveTextures(unsigned int count)
{
	this->m_Texture.reserve(count);
}

void Renderer::BindTextureToSlot(unsigned int idx, unsigned int slot)
{
	if (idx >= this->m_Texture.size()) return;
	if (slot < 0 || slot > 30) return;
	if (this->m_TextureSlotsUsed[slot] == 1) return;

	unsigned int _slot = slot + 2;
	this->m_Texture[idx].Bind(_slot);
	this->m_Shader.SetUniform1f("u_tex" + std::to_string(slot), (float)_slot);
	this->m_TextureSlotsUsed[slot] = 1;
}

void Renderer::BindTexture(unsigned int idx)
{
	if (idx >= this->m_Texture.size() || idx >= 31) return;
	if (this->m_TextureSlotsUsed[idx] == 1)
	{
		YEASTEM_WARNING("Slot Has Already Been Bound To. ");
		return;
	}

	unsigned int slot = idx + 2;
	this->m_Texture[idx].Bind(slot);
	this->m_Shader.SetUniform1i("u_tex" + std::to_string(idx), (int)slot);
	//std::cout << "?Texture " << this->m_Texture[idx].getFileName() << " Bound to " << "u_tex" + std::to_string(idx) << std::endl;

	this->m_TextureSlotsUsed[idx] = 1;
}

void Renderer::UnbindTextures()
{
	for (int i = 0; i < this->m_Texture.size(); i++)
	{
		unsigned int slot = this->m_Texture[i].getBoundSlot();
		if (!slot) continue;

		this->m_TextureSlotsUsed[slot - 2] = 0;
		//std::cout << slot << std::endl;
		//this->m_Texture[i].Unbind();
	}
}

YEASTEM_END
