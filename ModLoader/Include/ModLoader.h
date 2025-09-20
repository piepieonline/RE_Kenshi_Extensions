#include <vector>

#include "../Include/KenshiLibHelper.h"
#include <MyGUI_RotatingSkin.h>

#include <lua.hpp>
#include "LuaBridge/LuaBridge.h"

#include <_IncompleteTypes.h>
#include <kenshi/Item.h>

#include "Hook.h"
#include "LoadedMod.h"

class ModLoader
{
public:
	static ModLoader& Instance();

	static void Log(const char* format, ...);

	void ReloadMods();

private:
	void ClearLog();

	std::vector<LoadedMod*> loadedMods;

	static std::map<std::string, HookBase*> HookMap;
	std::vector<HookBase*> hooks
	{
		new Hook<void, Kenshi::Ownerships*, int32_t>(
			"Ownerships_addMoney",
			"\x8B\x81\x88\x00\x00\x00\x3B\xC2\x7D\x07\x85\xD2\x78\x03\x32\xC0",
			"xxx???xxxxxxxxxx",
			+[](Kenshi::Ownerships* ownerships, int32_t price) -> void
			{
				HookFunction<void, Kenshi::Ownerships*, int32_t>(ModLoader::HookMap["Ownerships_addMoney"], ownerships, price);
			}
		),
		new Hook<Kenshi::Inventory*, Kenshi::Inventory*, uintptr_t>(
			"Inventory_ctr",
			0x74A510,
			+[](Kenshi::Inventory* _this, uintptr_t owner) -> Kenshi::Inventory*
			{
				return HookFunction<Kenshi::Inventory*, Kenshi::Inventory*, uintptr_t>(ModLoader::HookMap["Inventory_ctr"], _this, owner);
			}
		),
		new Hook<int32_t, Kenshi::Item*, int32_t>(
			"Item_getValue_ukn",
			"\x48\x89\x5c\x24\x08\x48\x89\x74\x24\x10\x57\x48\x83\xec\x30\x48\x8b\x01\x0f\xb6\xf2\x48\x8b\xd9\xff\x90\xb8\x02\x00\x00",
			"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			+[](Kenshi::Item* _this, int32_t quantity) -> int32_t
			{
				return HookFunction<int32_t, Kenshi::Item*, int32_t>(ModLoader::HookMap["Item_getValue_ukn"], _this, quantity);
			}
		),
		new Hook<uintptr_t, uintptr_t, Kenshi::Item*, uintptr_t, MyGUI::Widget*>(
			"ukn_createDraggableItem",
			"\x40\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8d\x6c\x24\xe1\x48\x81\xec\xd8\x00\x00\x00\x48\xc7\x45\x87\xfe\xff\xff\xff",
			"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			+[](uintptr_t _this, Kenshi::Item* item, uintptr_t gameData_maybe, MyGUI::Widget* parentWidget) -> uintptr_t
			{
				return HookFunction<uintptr_t, uintptr_t, Kenshi::Item*, uintptr_t, MyGUI::Widget*>(ModLoader::HookMap["ukn_createDraggableItem"], _this, item, gameData_maybe, parentWidget);
			}
		),
		new Hook<void, uintptr_t, uint32_t>(
			"InputHandler_keyDownEvent",
			"\x40\x53\x48\x83\xec\x30\x44\x8b\xc2\x48\x8b\xd9\x83\xfa\x2a\x74\x05\x83\xfa\x36\x75\x07\xc6\x81\xd9\x00\x00\x00\x01",
			"xxxxxxxxxxxxxxxxxxxxxxxxxxx",
			+[](uintptr_t input, uint32_t oisKeyCode) -> void
			{
				HookFunction<void, uintptr_t, uint32_t>(ModLoader::HookMap["InputHandler_keyDownEvent"], input, oisKeyCode);
			}
		),
		new Hook<void, uintptr_t, uint32_t>(
			"InputHandler_keyUpEvent",
			"\x0f\xb6\x81\xd8\x00\x00\x00\x4c\x8b\xd1\xf6\xd8\x0f\xb6\x81\xd9\x00\x00\x00\x45\x1b\xc9\x41\x81\xe1\x00\x02\x00\x00\xf6\xd8",
			"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
			+[](uintptr_t input, uint32_t oisKeyCode) -> void
			{
				HookFunction<void, uintptr_t, uint32_t>(ModLoader::HookMap["InputHandler_keyUpEvent"], input, oisKeyCode);
			}
		)
			/*
		new Hook<Kenshi::GUIWindow*, Kenshi::GUIWindow*>(
			"GUIWindow_ctr",
			0x6e1f50,
			+[](Kenshi::GUIWindow* _this) -> Kenshi::GUIWindow*
			{
				return HookFunction<Kenshi::GUIWindow*, Kenshi::GUIWindow*>(ModLoader::HookMap["GUIWindow_setTitle"], _this);
			},
			true
		),
		*/
	};

	ModLoader();
	ModLoader(const ModLoader&) = delete;
	ModLoader& operator=(const ModLoader&) = delete;

	void ValidateOffsets();
	void ValidateOffset(const char* name, size_t actual, size_t expected);

	template<typename T>
	struct ReturnHolder {
		T value;
		bool hasValue = false;

		void set(T v) { value = v; hasValue = true; }
		T get() const { return value; }
	};

	template<>
	struct ReturnHolder<void> {
		bool hasValue = false;
		void set() const {}
		void get() const {}
	};

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

	typedef const char* (__thiscall* GetNameFn)(void*);

	template <typename TReturn, typename... TArgs>
	static TReturn HookFunction(HookBase* hook, TArgs... args)
	{
		using traits = function_traits<TReturn(TArgs...)>;
		// Log("[Hook] %s called.", hook->hookName.c_str());

		bool continueOriginal = true;
		ReturnHolder<TReturn> returnValue;

		std::string prefixName = hook->hookName;
		std::string postfixName = hook->hookName + "_postfix";


		lua_State* luaState = ModLoader::Instance().loadedMods[0]->luaState; // TODO: Support multiple mods
		luabridge::LuaRef luaPrefixCallback = luabridge::getGlobal(luaState, prefixName.c_str());
		luabridge::LuaRef luaPostfixCallback = luabridge::getGlobal(luaState, postfixName.c_str());

		if (luaPrefixCallback.isFunction()) {
			try {
				// Call the lua hook
				auto ret = luaPrefixCallback(args...);

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

					// TODO: Does it crash if we call here without continueoriginal as a variable?

					// If returnValue is set, we will use that as the return value
					if constexpr (!std::is_same_v<TReturn, void>) {
						if (!ret["returnValue"].isNil())
						{
							returnValue = ReturnHolder<TReturn>();
							returnValue.set(ret["returnValue"].cast<TReturn>());
						}
					}
				}
			}
			catch (luabridge::LuaException const& e) {
				Log("Lua prefix exception: %s\n", e.what());
			}
		}

		if (continueOriginal)
		{
			if constexpr (std::is_same_v<typename traits::return_type, void>)
			{
				hook->reference.GetOriginal<TReturn(TArgs...)>()(std::forward<TArgs>(args)...);
			}
			else
			{
				if(!returnValue.hasValue)
					returnValue.set(hook->reference.GetOriginal<TReturn(TArgs...)>()(std::forward<TArgs>(args)...));
			}
		}

		// Postfix is simpler, only the return value can change
		if (luaPostfixCallback.isFunction()) {
			try {
				// Call the lua hook
				auto ret = luaPostfixCallback(args...);

				// If lua returned a table, we might need to do something
				if (ret.isTable())
				{
					// If returnValue is set, we will use that as the return value
					if constexpr (!std::is_same_v<TReturn, void>) {
						if (!ret["returnValue"].isNil())
						{
							returnValue = ReturnHolder<TReturn>();
							returnValue.set(ret["returnValue"].cast<TReturn>());
						}
					}
				}
			}
			catch (luabridge::LuaException const& e) {
				Log("Lua postfix exception: %s\n", e.what());
			}
		}

		using FirstT = std::decay_t<std::tuple_element_t<0, std::tuple<TArgs...>>>;
		using SecondT = std::decay_t<std::tuple_element_t<1, std::tuple<TArgs...>>>;
		auto first = std::get<0>(std::forward_as_tuple(args...));
		auto second = std::get<1>(std::forward_as_tuple(args...));

		
		if constexpr (std::is_same_v<SecondT, Kenshi::Item*>)
		{
			auto i = reinterpret_cast<Kenshi::Item*>(second);
			/*
			int index = 22; // vtable slot index

			// Declare function signature
			using MethodType = int(__fastcall*)(void* thisPtr);

			// Get vtable
			void** vtable = *(void***)second;

			// Fetch function pointer
			auto func = reinterpret_cast<MethodType>(vtable[index]);

			// Call it
			std::cout << "ID: " << func(second) << std::endl;
			*/
		}

		// TODO: Temp mod hot reload
		if constexpr (std::is_same_v<SecondT, uint32_t>)
		{
			if (prefixName == "InputHandler_keyDownEvent")
			{
				if (second == OIS::KC_P)
				{
					Instance().ReloadMods();
					ModLoader::Log("[ModLoader] Mods Reloaded");
				}
			}
		}

		// End todo

		if constexpr (!std::is_same_v<typename traits::return_type, void>)
		{
			return returnValue.get();
		}
	}
};