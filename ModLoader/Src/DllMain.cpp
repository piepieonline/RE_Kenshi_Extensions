#include <windows.h>

#include "../Include/ModLoader.h"

ModLoader* modLoader;

void dllmain()
{
	ModLoader::Instance();
}

DWORD WINAPI threadWrapper(LPVOID param)
{
    dllmain();
    return 0;
}

// Ogre plugin export
extern "C" void __declspec(dllexport) dllStartPlugin(void)
{
    CreateThread(NULL, 0, threadWrapper, 0, 0, 0);
}