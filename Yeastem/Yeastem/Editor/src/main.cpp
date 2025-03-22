#include "Engine/src/yst_pch.h"

#include "Engine/src/Core/Application.h"
#include "Engine/src/Platforms/Windows/Window.h"

#include "Engine/src/Core/Scene/Components.h"

YEASTEM_BEGIN

void AttachEntities()
{
	ResourceManager& resourceManager = Yeastem::Application::GetResourceManager();
	Scene& currentScene = Yeastem::Application::Get().CurrentScene;

	Entity paddle1 = currentScene.CreateEntity("Stem/Paddle1", "Paddle");
	Entity paddle2 = currentScene.CreateEntity("Stem/Paddle2", "Paddle");
	Entity ball = currentScene.CreateEntity("Stem/Ball", "Paddle");

	paddle1.AddComponent<TransformComponent>(Vector2{ 150.0f, 250.0f });
	paddle2.AddComponent<TransformComponent>(Vector2{ 850.0f, 250.0f });
	ball.AddComponent<TransformComponent>(Vector2{ 500.0f, 250.0f });

	RenderQuadComponent& renderCompPaddle1 = paddle1.AddComponent<RenderQuadComponent>();
	RenderQuadComponent& renderCompPaddle2 = paddle2.AddComponent<RenderQuadComponent>();
	RenderQuadComponent& renderCompBall = ball.AddComponent<RenderQuadComponent>();

	renderCompPaddle1.Size = { 40.0f, 200.0f };
	renderCompPaddle2.Size = { 40.0f, 200.0f };
	renderCompBall.Size = { 40.0f, 40.0f };

	renderCompPaddle1.Textures.reserve(1);
	renderCompPaddle2.Textures.reserve(1);
	renderCompBall.Textures.reserve(1);

	renderCompPaddle1.Textures.push_back(resourceManager.Textures.Load("PongTest/Paddle.png"));
	renderCompPaddle2.Textures.push_back(resourceManager.Textures.Load("PongTest/Paddle.png"));
	renderCompBall.Textures.push_back(resourceManager.Textures.Load("PongTest/Ball.png"));

	Renderer::SetQuadShader(resourceManager.Shaders.Load(
		"src/Shaders/Quad.vert", "src/Shaders/Quad.frag"
	));

	Entity scriptGame = currentScene.CreateEntity("Stem/Game", "Game");
	scriptGame.AddComponent<ScriptComponent>("PongTest/Game.lua");

	Entity scriptPanels = currentScene.CreateEntity("Stem/Panels", "Panels");
	scriptPanels.AddComponent<ScriptComponent>("PongTest/Panels.lua");

	Entity scriptRectCollisionModule = currentScene.CreateEntity("Stem/RectCollisionModule", "RectCollisionModule");
	scriptRectCollisionModule.AddComponent<ScriptComponent>("PongTest/RectCollisionModule.lua");
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

	WindowInfoData windowData;
	windowData.Title = "Yeastem!";
	windowData.Width = 1350;
	windowData.Height = 600;
	windowData.Flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	Application::Init(windowData);

#if YEASTEM_COLOURLESS
	MakeWindowTransparent(window, RGB(255, 0, 255));
	SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
#endif


	AttachEntities();

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

