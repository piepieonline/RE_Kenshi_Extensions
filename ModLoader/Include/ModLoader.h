
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

	template <size_t Index = 0, typename Tuple>
	static void AssignArgumentOverrides(const luabridge::LuaRef& ret, Tuple& t) {
		// Loop through each potential argument, apply it if it exists (The lua return args table is sparse and 1-indexed)
		if constexpr (Index < std::tuple_size<Tuple>::value) {
			if (!ret[Index + 1].isNil()) {
				std::get<Index>(t) = ret[Index + 1];
			}
			AssignArgumentOverrides<Index + 1>(ret, t);
		}
	}

	template <typename TReturn, typename... TArgs>
	static TReturn HookFunction(HookBase* hook, TArgs... args)
	{
		Log("[Hook] %s called.", hook->hookName.c_str());

		bool continueOriginal = true;
		bool hasReturnValue = false;
		void* returnValue;

		lua_State* luaState = ModLoader::Instance().loadedMods[0]->luaState; // TODO: Support multiple mods
		luabridge::LuaRef luaCallback = luabridge::getGlobal(luaState, hook->hookName.c_str());

		if (luaCallback.isFunction()) {
			try {
				// Call the lua hook
				auto ret = luaCallback(args...);

				// If lua returned a table, we might need to do something
				if (ret.isTable())
				{
					// Args - override the args with the given indexes in lua
					if(ret["args"].isTable()) {
						AssignArgumentOverrides(ret["args"], std::tie(args...));
					}

					// If continueOriginal is set to false, we won't call the original function
					// Unless we are missing a return value and one is required
					if(continueOriginal && ret["continueOriginal"].isBool() && !ret["continueOriginal"].cast<bool>())
						continueOriginal = false;

					// If returnValue is set, we will use that as the return value
					if constexpr (!std::is_same_v<TReturn, void>) {
						if (!ret["returnValue"].isNil())
						{
							hasReturnValue = true;
							returnValue = ret["returnValue"].cast<TReturn>();
						}
					}
				}
			}
			catch (luabridge::LuaException const& e) {
				Log("Lua exception: %s\n", e.what());
			}
		}

		using traits = function_traits<TReturn(TArgs...)>;
		if constexpr (std::is_same_v<typename traits::return_type, void>) {
			if (continueOriginal)
			{
				Log("[Hook] %s calling original (void function).", hook->hookName.c_str());
				hook->reference.GetOriginal<TReturn(TArgs...)>()(args...);
			}
			else
			{
				Log("[Hook] %s skipping original (void function).", hook->hookName.c_str());
			}
		}
		else
		{
			// TODO: Log case that we don't have a return value but have been told to not call original
			if (continueOriginal || !hasReturnValue)
			{
				if (hasReturnValue)
				{
					Log("[Hook] %s calling original but overwriting return value.", hook->hookName.c_str());
					hook->reference.GetOriginal<TReturn(TArgs...)>()(args...);
					return *(TReturn*)returnValue;
				}
				else
				{
					Log("[Hook] %s calling original and using return value.", hook->hookName.c_str());
					return hook->reference.GetOriginal<TReturn(TArgs...)>()(args...);
				}
			}
			else
			{
				Log("[Hook] %s skipping original and using return value.", hook->hookName.c_str());
				return *(TReturn*)returnValue;
			}
		}
	}
};