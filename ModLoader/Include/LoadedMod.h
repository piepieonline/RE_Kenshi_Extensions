#pragma once

#include <string>

#include <lua.hpp>

class LoadedMod
{
public:
	LoadedMod(std::string directory);
	~LoadedMod();

	void ReloadLuaFile();
	
	bool UsesHook(const std::string& hookName);
	
	bool usesLua = false;
	lua_State* luaState;

private:
	std::string modName;
};