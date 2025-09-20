#pragma once

#include "../Include/KenshiLibHelper.h"

#include <windows.h>
#include <MyGUI_RotatingSkin.h>

MyGUI::Widget* KenshiLib_Wrappers::MyGUI_Helpers::FindWidgetRecursive(std::string name, bool waitForWidget)
{
	HMODULE hLib = LoadLibraryW(L"KenshiLib.dll");
	if (!hLib) {
		DWORD err = GetLastError();
		printf("LoadLibrary failed with 0x%08X\n", err);
		return 0;
	}

	DllFindWidgetRecursive pFindWidgetRecursive = (DllFindWidgetRecursive)GetProcAddress(hLib, "MyGUI_Helper_FindWidget");
	return (MyGUI::Widget*)pFindWidgetRecursive(name.c_str(), waitForWidget);
}

MyGUI::RotatingSkin* KenshiLib_Wrappers::MyGUI_Helpers::GetRotatingSkin(MyGUI::Widget* widget)
{
	HMODULE hLib = LoadLibraryW(L"KenshiLib.dll");
	if (!hLib) {
		DWORD err = GetLastError();
		printf("LoadLibrary failed with 0x%08X\n", err);
		return 0;
	}

	Dll_MyGUI_GetRotatingSkin pMyGUI_GetRotatingSkin = (Dll_MyGUI_GetRotatingSkin)GetProcAddress(hLib, "MyGUI_Helper_GetRotatingSkin");
	return (MyGUI::RotatingSkin*)pMyGUI_GetRotatingSkin((void*)widget);
}