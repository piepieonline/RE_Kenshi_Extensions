#pragma once

#include <string>
#include <lua.hpp>

class LuaBridgeSetup
{
public:
	static int LuaPrint(lua_State* L);
	static void CreateLuaState(lua_State* L, std::string modName);

private:
	static void CreateLuaStateInternal(lua_State* L);
};