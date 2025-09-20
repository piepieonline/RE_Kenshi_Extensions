#include "../Include/LuaBridgeSetup.h";

#include "Lua.h"
#include "LuaBridge/LuaBridge.h"

#include "../Include/ModLoader.h"

void LuaBridgeSetup::CreateLuaState(lua_State* L, std::string modName)
{
    lua_pushcfunction(L, &LuaPrint);
    lua_setglobal(L, "print");

    lua_pushstring(L, modName.c_str());
    lua_setglobal(L, "ModName");

    LuaBridgeSetup::CreateLuaStateInternal(L);
}

int LuaBridgeSetup::LuaPrint(lua_State* L)
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
