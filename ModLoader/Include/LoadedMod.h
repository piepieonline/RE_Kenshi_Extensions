#pragma once

#include <string>

#include <lua.hpp>

class LoadedMod
{
public:
	LoadedMod(std::string directory);
	~LoadedMod();
	
	bool UsesHook(const std::string& hookName);
	
	bool usesLua = false;
	lua_State* luaState;


private:
	static int LuaPrint(lua_State* L);

	void CreateLuaState();

	std::string modName;
};