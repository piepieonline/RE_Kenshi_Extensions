#include "../Include/LoadedMod.h"

#include <filesystem>

#include <lua.hpp>
#include "LuaBridge/LuaBridge.h"
#include "../Include/LuaBridgeSetup.h"

#include "../Include/ModLoader.h"

LoadedMod::LoadedMod(std::string directory)
{
    modName = directory;
    
	// TODO: Execute a DLL if it exists in the mod directory first
    // Log an error if the lua script is invalid

    std::string luaScriptFileName = "./scriptExtensions/" + modName + "/kenshi_main.lua";

    if (std::filesystem::exists(luaScriptFileName))
    {
        luaState = luaL_newstate();

        luaL_openlibs(luaState);

        LuaBridgeSetup::CreateLuaState(luaState, modName);

        luaL_dofile(luaState, luaScriptFileName.c_str());

        usesLua = true;
    }
}

LoadedMod::~LoadedMod()
{
    if (usesLua)
    {
        lua_close(luaState);
        usesLua = false;
    }
}

void LoadedMod::ReloadLuaFile()
{
    // TODO: What happens if we have global references? Could that break something?
    if (usesLua)
    {
        std::string luaScriptFileName = "./scriptExtensions/" + modName + "/kenshi_main.lua";
        luaL_dofile(luaState, luaScriptFileName.c_str());
    }
}

bool LoadedMod::UsesHook(const std::string& hookName)
{
    if (usesLua)
    {
        std::string prefixName = hookName;
		std::string postfixName = hookName + "_postfix";

        luabridge::LuaRef luaPrefix = luabridge::getGlobal(luaState, prefixName.c_str());
        luabridge::LuaRef luaPostfix = luabridge::getGlobal(luaState, postfixName.c_str());

        if (
            luaPrefix.isFunction() ||
            luaPostfix.isFunction()
        ) {
			return true;
        }
    }

    return false;
}