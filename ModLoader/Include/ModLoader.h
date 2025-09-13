
#include <vector>

#include <lua.hpp>
#include "LuaBridge/LuaBridge.h"

#include <_IncompleteTypes.h>

#include "Hook.h"
#include "LoadedMod.h"

class ModLoader
{
public:
	static ModLoader& Instance();

	static void Log(const char* format, ...);

private:
	std::vector<LoadedMod*> loadedMods;

	static std::map<std::string, HookBase*> HookMap;
	std::vector<HookBase*> hooks
	{
		new Hook<void, Kenshi::Ownerships*, int32_t>(
			"Ownerships_addMoney",
			6,
			"\x8B\x81\x88\x00\x00\x00\x3B\xC2\x7D\x07\x85\xD2\x78\x03\x32\xC0",
			"xxx???xxxxxxxxxx",
			+[](Kenshi::Ownerships* ownerships, int32_t price) -> void
			{
				HookFunction<void, Kenshi::Ownerships*, int32_t>(ModLoader::HookMap["Ownerships_addMoney"], ownerships, price);
			}
		),
		new Hook<Kenshi::Inventory*, Kenshi::Inventory*, uintptr_t>(
			"Inventory_ctr",
			6,
			0x74A510,
			+[](Kenshi::Inventory* _this, uintptr_t owner) -> Kenshi::Inventory*
			{
				return HookFunction<Kenshi::Inventory*, Kenshi::Inventory*, uintptr_t>(ModLoader::HookMap["Inventory_ctr"], _this, owner);
			}
		)
	};

	ModLoader();
	ModLoader(const ModLoader&) = delete;
	ModLoader& operator=(const ModLoader&) = delete;

	void ValidateOffsets();
	void ValidateOffset(const char* name, size_t actual, size_t expected);

	template <typename TReturn, typename... TArgs>
	static TReturn HookFunction(HookBase* hook, TArgs... args)
	{
		Log("[Hook] %s called.", hook->hookName.c_str());

		lua_State* luaState = ModLoader::Instance().loadedMods[0]->luaState; // TODO: Support multiple mods
		luabridge::LuaRef luaCallback = luabridge::getGlobal(luaState, hook->hookName.c_str());

		if (luaCallback.isFunction()) {
			try {
				luaCallback(args...);
			}
			catch (luabridge::LuaException const& e) {
				Log("Lua exception: %s\n", e.what());
			}
		}

		using traits = function_traits<TReturn(TArgs...)>;
		if constexpr (std::is_same_v<typename traits::return_type, void>) {
			hook->reference.GetOriginal<TReturn(TArgs...)>()(args...);
		}
		else {
			return hook->reference.GetOriginal<TReturn(TArgs...)>()(args...);
		}
	}
};