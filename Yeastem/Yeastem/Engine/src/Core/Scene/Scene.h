#pragma once

#include "Core/OpenGL/Include.h"
#include "Core/Scene/Events.h"
#include "LuaAPI/Lua.h"
#include "Core/Scene/Profiling.h"

YEASTEM_BEGIN

class Scene
{
public:
	Scene() {}

public:
	void Init(ResourceManager& resourceManager);
	void Lua_Init();
	void Lua_AttachCFunction(int(*nativeFunction)(lua_State*), const char* functionName);
	void Lua_ExcecuteScript(const std::string& file);
	void Lua_AttachScript(const std::string& file);
	void Update(float DeltaTime);
	void Render(ResourceManager& resourceManager);
	void ProcessEvent(SDL_Event& evt);

	ObjectID CreateObject();
	Shape<QuadVertex>& GetObjectWithID(ObjectID id);

	void CreateFrameBuffer(uint32_t width, uint32_t height);
	void RecreateFrameBuffer(uint32_t width, uint32_t height);

	bool DoesFrameBufferExist() const { return m_FrameBuffer != nullptr; }
	uint32_t GetFrameBufferColourAttachmentID() const { return m_FrameBuffer->GetColourAttachmentID(); }
	uint32_t GetFrameBufferDepthAttachmentID() const { return m_FrameBuffer->GetDepthAttachmentID(); }

	void SetQuadShader(ObjectID shaderID) { m_Renderer.SetQuadShader(shaderID); }

public:
	bool IsRunning = false;
	bool IsReset = true;
	bool ShouldRefreshFrameBuffer = false;

	uint64_t CurrentTime = 0;
	ImVec2 SceneSize;

private:
	void UpdateObjectFromScripts(ObjectID shapeIdx);
	void UpdateScriptsFromObjects(ObjectID shapeIdx);
	void AddKeyEnum(const char* keyName, uint32_t keyCode);
	void Lua_RebuildState();
	void CreateObjectInScript(Shape<QuadVertex>& shape, const std::string& str);

private:
	Yeastem::EventHandler m_EventHandler;
	BatchRenderer m_Renderer;
	LuaState m_LuaState;
	int m_ScriptCount = -1; // -1 Represents Uninitialised

	std::unique_ptr<FrameBuffer> m_FrameBuffer;

	std::vector<std::string> m_Scripts;

	std::unordered_map<ObjectID, Shape<QuadVertex>> m_Shapes;
};

YEASTEM_END

