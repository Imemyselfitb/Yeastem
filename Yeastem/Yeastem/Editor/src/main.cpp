#include "Yeastem.h"
#include "ProjectManager.h"

#include "Shaders/Shaders.h"

#include <chrono>
#include <thread>

YEASTEM_BEGIN

void AttachEntities(bool isLaunched = false)
{
	ResourceManager& resourceManager = Yeastem::Application::Get().GetResourceManager();
	Renderer::SetQuadShader(resourceManager.Shaders.Load(
		Shaders::Quad_VERT, Shaders::Quad_FRAG
	));

	if (!isLaunched)
	{
		SDL_DialogFileFilter filters[] = {
			{ "Yeastem Project (*.yeast)", "yeast" }
		};
		FileIO::FileSelect::open(filters, 1, 'PRJ');
	}

	ProjectManager::Get().InitPanels();
}

YEASTEM_END

int main(int argc, char* argv[])
{
	for (int i = 0; i < argc; i++)
	{
		std::string arg = argv[i];
		printf("String: %s\n", arg.c_str());
	}

	using namespace Yeastem;

	WindowInfoData windowData;
	windowData.Title = "Yeastem!";
	windowData.Size = { 1350, 600 };
	windowData.Flags = SDL_WINDOW_RESIZABLE;
	Application::Get().Init(windowData);

#if YEASTEM_COLOURLESS
	MakeWindowTransparent(window, RGB(255, 0, 255));
	SDL_SetWindowAlwaysOnTop(window, SDL_TRUE);
#endif

	AttachEntities();

	SDL_Event currentEvent;
	while (FileIO::FileSelect::isOpen)
	{
		while (SDL_PollEvent(&currentEvent));
		SDL_GL_SwapWindow(Application::Get().GetWindow());
	}
	
	if (FileIO::FileSelect::isSuccess)
	{
		ProjectManager::Get().LoadProject(FileIO::FileSelect::selectedPaths[0]);
		Application::Get().Run();
	}

	return 0;
}

#if defined(YST_PLATFORM_WINDOWS) && defined(YST_CONFIG_DIST)

#undef APIENTRY
#include <windows.h>

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	int argc;
	LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argvW == NULL)
	{
		MessageBox(NULL, L"Failed to parse command line", L"Error", MB_OK | MB_ICONERROR);
		return 1;
	}

	// Allocate array of char* for converted arguments
	char** argv = new char* [argc];
	for (int i = 0; i < argc; ++i)
	{
		// Convert each wide string to a narrow (multi-byte) string
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
		argv[i] = new char[size_needed];
		WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], size_needed, NULL, NULL);
	}

	int statusCode = main(argc, argv);

	LocalFree(argv);
	return 0;
}
#endif

