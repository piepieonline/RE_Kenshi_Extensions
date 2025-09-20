#pragma once

#include <windows.h>
#include <MyGUI_RotatingSkin.h>

namespace KenshiLib_Wrappers
{
	namespace MyGUI_Helpers
	{
        typedef const void* (*DllFindWidgetRecursive)(const char* name, bool waitForWidget);
        MyGUI::Widget* FindWidgetRecursive(std::string name, bool waitForWidget);

        typedef const void* (*Dll_MyGUI_GetRotatingSkin)(void* widget);
        MyGUI::RotatingSkin* GetRotatingSkin(MyGUI::Widget* widget);
	}
}