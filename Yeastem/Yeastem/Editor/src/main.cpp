#include "Yeastem.h"
#include "ProjectManager.h"

YEASTEM_BEGIN

void AttachEntities()
{
	ResourceManager& resourceManager = Yeastem::Application::Get().GetResourceManager();
	Renderer::SetQuadShader(resourceManager.Shaders.Load(
		"src/Shaders/Quad.vert", "src/Shaders/Quad.frag"
	));

	ProjectManager::LoadProject("PongTest/project.yeast", "PongTest/");

	ProjectManager::InitPanels();
}

YEASTEM_END

int main(int argc, char* argv[])
{
	using namespace Yeastem;

	WindowInfoData windowData;
	windowData.Title = "Yeastem!";
	windowData.Width = 1350;
	windowData.Height = 600;
	windowData.Flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	Application::Get().Init(windowData);

#if YEASTEM_COLOURLESS
	MakeWindowTransparent(window, RGB(255, 0, 255));
	SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
#endif

	AttachEntities();

	Application::Get().Run();

	for (auto& window : Yeastem::AllWindows)
	{
		std::cout << ">>>" << window.Title << "\n";
		std::cout << "(" << window.Position << ")" << "\n";
		std::cout << "(" << window.Size << ")" << "\n";
	}

	return 0;
}

