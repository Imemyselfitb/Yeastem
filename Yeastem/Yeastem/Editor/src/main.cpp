#include "Engine/src/yst_pch.h"

#include "Engine/src/Core/Application.h"
#include "Engine/src/Platforms/Windows/Window.h"

YEASTEM_BEGIN
void AttachObjects()
{
	ResourceManager& resourceManager = Yeastem::Application::GetResourceManager();
	Scene& currentScene = Yeastem::Application::Get().CurrentScene;

	ObjectID Paddle1ID = currentScene.CreateObject();
	ObjectID Paddle2ID = currentScene.CreateObject();
	ObjectID BallID = currentScene.CreateObject();

	std::vector<QuadVertex> paddleVertices = {
		{ { -20.0f, -100.0f }, { 0.0f, 0.0f } },
		{ {  20.0f, -100.0f }, { 1.0f, 0.0f } },
		{ {  20.0f,  100.0f }, { 1.0f, 1.0f } },
		{ { -20.0f,  100.0f }, { 0.0f, 1.0f } }
	};

	std::vector<QuadVertex> ballVertices = {
		{ { -20.0f, -20.0f }, { 0.0f, 0.0f } },
		{ {  20.0f, -20.0f }, { 1.0f, 0.0f } },
		{ {  20.0f,  20.0f }, { 1.0f, 1.0f } },
		{ { -20.0f,  20.0f }, { 0.0f, 1.0f } }
	};

	Shape<QuadVertex>& Paddle1 = currentScene.GetObjectWithID(Paddle1ID);
	Shape<QuadVertex>& Paddle2 = currentScene.GetObjectWithID(Paddle2ID);
	Shape<QuadVertex>& Ball = currentScene.GetObjectWithID(BallID);

	Paddle1.AssignVertices(paddleVertices);
	Paddle2.AssignVertices(paddleVertices);
	Ball.AssignVertices(ballVertices);

	Paddle1.DefaultPosition = Vector2(150.0f, 250.0f);
	Paddle2.DefaultPosition = Vector2(850.0f, 250.0f);
	Ball.DefaultPosition = Vector2(500.0f, 250.0f);

	Paddle1.DefaultScale = 1.0f;
	Paddle2.DefaultScale = 1.0f;
	Ball.DefaultScale = 1.0f;

	Paddle1.ReserveTextures(1);
	Paddle1.AddTexture("PongTest/Paddle.png", resourceManager);
	
	Paddle2.ReserveTextures(1);
	Paddle2.AddTexture("PongTest/Paddle.png", resourceManager);
	
	Ball.ReserveTextures(1);
	Ball.AddTexture("PongTest/Ball.png", resourceManager);

	currentScene.SetQuadShader(resourceManager.Shaders.Load(
		"src/Shaders/Quad.vert", "src/Shaders/Quad.frag"
	));

	currentScene.Lua_AttachScript("PongTest/Panels.lua");
	currentScene.Lua_AttachScript("PongTest/Game.lua");
	currentScene.Lua_AttachScript("PongTest/RectCollisionModule.lua");
}

YEASTEM_END

int main(int argc, char* argv[])
{
	using namespace Yeastem;

	Application::SetupGLFlags();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not be initialized: " << SDL_GetError();
	}

	int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	SDL_Window* window = SDL_CreateWindow(
		"Yeastem!",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1350, 600, flags
	);

#if YEASTEM_COLOURLESS
	MakeWindowTransparent(window, RGB(255, 0, 255));
	SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
#endif

	Application::AttachWindow(window);
	Application::CreateGLContext();
	Application::InitImGui();

	AttachObjects();

	Application::Run();

	for (auto& window : Yeastem::AllWindows)
	{
		std::cout << ">>>" << window.Title << "\n";
		std::cout << "(" << window.Position << ")" << "\n";
		std::cout << "(" << window.Size << ")" << "\n";
	}

	Application::DestroyWindow();

	SDL_Quit();

	return 0;
}

