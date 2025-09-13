#include "../Include/LoadedMod.h"

#include <filesystem>

#include <lua.hpp>
#include "LuaBridge/LuaBridge.h"

#include <kenshi/Kenshi.h>
#include <kenshi/GameWorld.h>
#include <kenshi/ModInfo.h>
#include <kenshi/Faction.h>
#include <kenshi/PlayerInterface.h>

#include <_IncompleteTypes.h>

#include "../Include/ModLoader.h"

LoadedMod::LoadedMod(std::string directory)
{
    modName = directory;
    std::string luaScriptFileName = "./modPlugins/" + directory + "/kenshi_main.lua";

	// TODO: Execute a DLL if it exists in the mod directory first
    // Log an error if the lua script is invalid

    luaState = luaL_newstate();
    if (std::filesystem::exists(luaScriptFileName))
    {
        luaL_openlibs(luaState);

        lua_pushcfunction(luaState, &LuaPrint);
        lua_setglobal(luaState, "print");

		lua_pushstring(luaState, modName.c_str());
        lua_setglobal(luaState, "ModName");

        CreateLuaState();

        luaL_dofile(luaState, luaScriptFileName.c_str());

        usesLua = true;
    }
}

void LoadedMod::CreateLuaState()
{
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Kenshi")

        .beginClass<Kenshi::Ownerships>("Ownerships")
        .addProperty("money", &Kenshi::Ownerships::money, false)
        .endClass()

        .beginClass<Kenshi::ActivePlatoon>("ActivePlatoon")
        .addProperty("platoonMemberCount", &Kenshi::ActivePlatoon::platoonMemberCount, false)
        .endClass()

        .beginClass<Kenshi::Platoon>("Platoon")
        .addProperty("currentActivePlatoon", &Kenshi::Platoon::currentActivePlatoon, false)
        .endClass()

        .beginClass<Kenshi::PlayerInterface>("PlayerInterface")
        .addProperty("currentPlatoon", &Kenshi::PlayerInterface::currentPlatoon, false)
        .endClass()

        .beginClass<Kenshi::GameWorld>("GameWorld")
        .addProperty("player", &Kenshi::GameWorld::player, false)
        .endClass()

        // Global accessor to the singleton
        .addFunction("GetGameWorld", Kenshi::GetGameWorld)
        .endNamespace();
}

LoadedMod::~LoadedMod()
{
    lua_close(luaState);
}

bool LoadedMod::UsesHook(const std::string& hookName)
{
    if (usesLua)
    {
        luabridge::LuaRef luaCallback = luabridge::getGlobal(luaState, hookName.c_str());

        if (luaCallback.isFunction()) {
			return true;
        }
    }

    return false;
}

int LoadedMod::LuaPrint(lua_State* L)
{
    std::string output;

    int nargs = lua_gettop(L);
    for (int i = 1; i <= nargs; ++i) {
        output += luaL_tolstring(L, i, nullptr);
        output += '\t';
        lua_pop(L, 1);
    }

    lua_getglobal(L, "ModName");
	std::string modName = lua_tostring(L, -1);

    ModLoader::Log("[Lua - %s] %s", modName.c_str(), output.c_str());

    return 0;
}
