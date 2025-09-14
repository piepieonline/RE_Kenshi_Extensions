#include "../Include/ModLoader.h"

#include <windows.h>

#include <lua.hpp>
#include "LuaBridge/LuaBridge.h"

#include <Escort.h>

#include <core/Scanner.h>
#include <kenshi/Kenshi.h>
#include <kenshi/GameWorld.h>
#include <kenshi/ModInfo.h>
#include <kenshi/Faction.h>
#include <kenshi/PlayerInterface.h>

#include <_IncompleteTypes.h>

std::map<std::string, HookBase*> ModLoader::HookMap{};

ModLoader& ModLoader::Instance()
{
    static ModLoader instance;
    return instance;
}

ModLoader::ModLoader()
{
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);

    Kenshi::BinaryVersion gameVersion = Kenshi::GetKenshiVersion();

    if(gameVersion.GetPlatform() == Kenshi::BinaryVersion::KenshiPlatform::UNKNOWN) {
        Log("[ModLoader] Unknown Kenshi version, mod loader will not run.");
        return;
    }

    uintptr_t moduleBase;
    size_t moduleSize;
    GetModuleCodeRegion(gameVersion.GetBinaryName().c_str(), &moduleBase, &moduleSize);

    Log("[ModLoader] Initialized on game version %s. PID: %d. Module Base: 0x%p. GameWorld address: 0x%p", gameVersion.GetVersion().c_str(), GetCurrentProcessId(), moduleBase, &Kenshi::GetGameWorld());

    ValidateOffsets();

	// TODO: Load mods from subdirectory
    loadedMods = {
        new LoadedMod("PriceDoubleMod")
	};

    // Only create hooks that are actually needed
    for (auto hook : hooks) {
        bool loadHook = hook->alwaysLoad;
        if (!loadHook)
        {
            for (auto mod : loadedMods)
            {
                if (mod->UsesHook(hook->hookName))
                {
                    loadHook = true;
					break;
                }
            }
        }

        if (loadHook)
        {
            hook->CreateHook();
		    HookMap[hook->hookName] = hook;
            Log("[ModLoader] Hook %s installed at offset 0x%p", hook->hookName.c_str(), (void*)hook->reference.offset);
        }
        else
        {
            Log("[ModLoader] Hook %s skipped as unused", hook->hookName.c_str());
        }
	}
}

void ModLoader::Log(const char* format, ...) {
    char buffer[1024]; // TODO: temporary buffer for formatted string

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    std::cout << buffer << std::endl;
}

void ModLoader::ValidateOffsets() {
#if _MSC_VER
    ValidateOffset("GameDataContainer::gamedataID", offsetof(Kenshi::GameDataContainer, gamedataID), 0x50);
    ValidateOffset("GameWorld::player", offsetof(Kenshi::GameWorld, player), 0x580);


    ValidateOffset("PlayerInterface::selectBox", offsetof(Kenshi::PlayerInterface, selectBox), 0xA8);
    ValidateOffset("PlayerInterface::mRightDown", offsetof(Kenshi::PlayerInterface, mRightDown), 0x2F4);

    ValidateOffset("Character::naturalWeapon", offsetof(Kenshi::Character, naturalWeapon), 0x6D0);

    ValidateOffset("SelectionBox::startPos", offsetof(Kenshi::SelectionBox, startPos), 0x00);
    ValidateOffset("SelectionBox::widget", offsetof(Kenshi::SelectionBox, widget), 0x30);

    ValidateOffset("Ownerships::money", offsetof(Kenshi::Ownerships, money), 0x88);
#endif
}

void ModLoader::ValidateOffset(const char* name, size_t actual, size_t expected) {
    if (actual != expected) {
        std::cerr << "[!] Offset mismatch for " << name
            << ". Expected 0x" << std::hex << expected
            << ", Actual 0x" << actual << "\n";
    }
}