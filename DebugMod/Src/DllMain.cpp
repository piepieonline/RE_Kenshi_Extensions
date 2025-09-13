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


void (*Ownerships_addMoney_orig)(Kenshi::Ownerships* ownerships, int32_t price);
Kenshi::Inventory* (*Inventory_ctr_orig)(Kenshi::Inventory* _this, uintptr_t owner);

lua_State* luaState;

void DoLog(const char* format, ...) {
    char buffer[1024]; // temporary buffer for formatted string

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    std::cout << buffer << std::endl;
}


void WaitForMainMenu()
{
    MyGUI::Gui* gui = nullptr;
    while (gui == nullptr)
    {
        gui = MyGUI::Gui::getInstancePtr();
        Sleep(10);
    }
    MyGUI::WidgetPtr versionText = nullptr;
    while (versionText == nullptr)
    {
        versionText = Kenshi::FindWidget(gui->getEnumerator(), "VersionText");
        Sleep(10);
    }
}

uintptr_t PatternScan(uintptr_t base, size_t size, const char* pattern, const char* mask)
{
    size_t patternLen = strlen(mask);

    for (size_t i = 0; i < size - patternLen; i++) {
        bool found = true;
        for (size_t j = 0; j < patternLen; j++) {
            if (mask[j] == 'x' && pattern[j] != *(char*)(base + i + j)) {
                found = false;
                break;
            }
        }
        if (found) {
            return i;
        }
    }
    return 0;
}

bool isGlobalDefined(lua_State* L, const char* name) {
    lua_getglobal(L, name);            // Push global onto stack
    bool exists = !lua_isnil(L, -1);   // Check if it's not nil
    lua_pop(L, 1);                     // Remove from stack
    return exists;
}

void addMoneyHook(Kenshi::Ownerships* ownerships, int32_t price)
{
    DoLog("[DebugMod] In the money hook");

    luabridge::LuaRef Ownerships_addMoney = luabridge::getGlobal(luaState, "Ownerships_addMoney");

    if (Ownerships_addMoney.isFunction()) {
        try {
            Ownerships_addMoney(ownerships, price);
        }
        catch (luabridge::LuaException const& e) {
            DoLog("Lua exception: %s\n", e.what());
        }
    }

	Kenshi::GameWorld* gameWorld = &Kenshi::GetGameWorld();
    for (int i = 0; i < gameWorld->player->currentPlatoon->currentActivePlatoon->platoonMemberCount; i++)
        DoLog("\t%s", gameWorld->player->currentPlatoon->currentActivePlatoon->platoonMembers->data[i].character->displayName);

    Ownerships_addMoney_orig(ownerships, price * 2);
}

Kenshi::Inventory* inventoryCtorHook(Kenshi::Inventory* _this, uintptr_t owner)
{
    luabridge::LuaRef Inventory_ctr = luabridge::getGlobal(luaState, "Inventory_ctr");

    if (Inventory_ctr.isFunction()) {
        try {
            DoLog("[DebugMod] Inventory_ctr function found in lua", (void*)owner);
        }
        catch (luabridge::LuaException const& e) {
            DoLog("Lua exception: %s\n", e.what());
        }
    }
    else
    {
        DoLog("[DebugMod] Inventory_ctr function NOT found in lua", (void*)owner);
    }

    return Inventory_ctr_orig(_this, owner);

    DoLog("[DebugMod] In the inventory ctor hook? Owner: 0x%p", (void*)owner);
    const char* ownerName = "<unknown>";

    // Read int at owner + 0x28
    int val = *reinterpret_cast<int*>(owner + 0x28);

    if (val < 16)
    {
        // If < 16, interpret (owner + 0x18) as ASCII string directly
        ownerName = reinterpret_cast<const char*>(owner + 0x18);
    }
    else
    {
        // Otherwise, (owner + 0x18) points to a string
        char* strPtr = *reinterpret_cast<char**>(owner + 0x18);
        if (strPtr != nullptr)
            ownerName = strPtr;
    }

    DoLog("[DebugMod] Inventory %p assigned to %s", _this, ownerName);

    return Inventory_ctr_orig(_this, owner);
}

void validateOffset(const char* name, size_t actual, size_t expected) {
    if (actual != expected) {
        std::cerr << "[!] Offset mismatch for " << name
            << ". Expected 0x" << std::hex << expected
            << ", Actual 0x" << actual << "\n";
    }
    else {
        std::cout << "[+] Offset OK: " << name
            << " at 0x" << std::hex << actual << "\n";
    }
}

void dllmain()
{
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);

    Kenshi::BinaryVersion gameVersion = Kenshi::GetKenshiVersion();
    uintptr_t moduleBase;
    size_t moduleSize;
    GetModuleCodeRegion(gameVersion.GetBinaryName().c_str(), &moduleBase, &moduleSize);

    DoLog("[DebugMod] Initialized. PID: %d. Module Base: 0x%p. GameWorld address: 0x%p", GetCurrentProcessId(), moduleBase, &Kenshi::GetGameWorld());

    validateOffset("GameDataContainer::gamedataID", offsetof(Kenshi::GameDataContainer, gamedataID), 0x50);
    validateOffset("GameWorld::player", offsetof(Kenshi::GameWorld, player), 0x580);


    validateOffset("PlayerInterface::selectBox", offsetof(Kenshi::PlayerInterface, selectBox), 0xA8);
    validateOffset("PlayerInterface::mRightDown", offsetof(Kenshi::PlayerInterface, mRightDown), 0x2F4);

    validateOffset("Character::naturalWeapon", offsetof(Kenshi::Character, naturalWeapon), 0x6D0);

    validateOffset("SelectionBox::startPos", offsetof(Kenshi::SelectionBox, startPos), 0x00);
    validateOffset("SelectionBox::widget", offsetof(Kenshi::SelectionBox, widget), 0x30);

    const char* purchase_item_pattern = "\x8B\x81\x88\x00\x00\x00\x3B\xC2\x7D\x07\x85\xD2\x78\x03\x32\xC0";
    const char* purchase_item_mask = "xxx???xxxxxxxxxx";

    uintptr_t addMoneyAddress = moduleBase + PatternScan(moduleBase, moduleSize, purchase_item_pattern, purchase_item_mask);

    Ownerships_addMoney_orig = Escort::JmpReplaceHook<void(Kenshi::Ownerships*, int32_t)>((void*)addMoneyAddress, addMoneyHook, 6);
	DoLog("[DebugMod] Ownerships_addMoney_orig hook installed at 0x%p (Offset: 0x%p)", (void*)addMoneyAddress, addMoneyAddress - moduleBase);
    uintptr_t inventoryCtrAddress = moduleBase + 0x74A510; /* + PatternScan(
        moduleBase,
        moduleSize,
        "\x48\x89\x4c\x24\x08\x56\x57\x41\x54\x41\x55\x41\x56\x48\x83\xec\x30\x48\xc7\x44\x24\x20\xfe\xff\xff\xff",
        "xxxxxxxxxxxxxxxxxxxxxxxxxx"
    );*/
	// Inventory_ctr_orig = Escort::JmpReplaceHook<Kenshi::Inventory*(Kenshi::Inventory*, uintptr_t)>((void*)inventoryCtrAddress, inventoryCtorHook, 6);
	DoLog("[DebugMod] Inventory_ctr_orig hook installed at 0x%p (Offset: 0x%p)", (void*)inventoryCtrAddress, inventoryCtrAddress - moduleBase);

    // void* LogManagerDestructorPtr = Escort::GetFuncAddress(".dll", "??1LogManager@MyGUI@@QEAA@XZ");

    /*
    MyGUI::LayoutManager mg;
    mg->
    */
     // loadLayout(const std::string & _file, const std::string & _prefix = "", Widget * _parent = nullptr);
    // void* LayoutManger_loadLoayout_ptr = Escort::GetFuncAddress("MyGUIEngine_x64.dll", "?loadLayout@LayoutManager@MyGUI@@QEAA");
    // LayoutManger_loadLoayout_orig = Escort::JmpReplaceHook<MyGUI::VectorWidgetPtr(const std::string & _file, const std::string & _prefix, MyGUI::Widget* widget)>(LayoutManger_loadLoayout_ptr, Terrain_getHeight_hook);
    
    luaState = luaL_newstate();
    luaL_openlibs(luaState);

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

	luaL_dofile(luaState, "modPlugins/script.lua");

    // lua_close(luaState);


    MyGUI::Gui* gui = nullptr;
    while (gui == nullptr)
    {
        // getInstancePtr returns pointer offset by -0x08
        auto rawPtr = MyGUI::Gui::getInstancePtr();
        if (rawPtr)
            gui = reinterpret_cast<MyGUI::Gui*>(reinterpret_cast<uintptr_t>(rawPtr));

        Sleep(10);
    }
    DoLog("[DebugMod] Gui pointer is at 0x%p", (void*)gui);

    /*
    MyGUI::Widget* widget = nullptr;
    while (widget == nullptr)
    {
        widget = gui->findWidgetT("VersionText", false);
        Sleep(10);
    }
    DoLog("[DebugMod] widget pointer is at 0x%p", (void*)widget);
    */

    // Crashing?
    // WaitForMainMenu();

    /*
    for (auto& mod : Kenshi::GetGameWorld().activeMods) {
        // mod is each element in activeMods
        DoLog("%s", mod->name);
    }
    */
}

DWORD WINAPI threadWrapper(LPVOID param)
{
    dllmain();
    return 0;
}

// Ogre plugin export
extern "C" void __declspec(dllexport) dllStartPlugin(void)
{
    CreateThread(NULL, 0, threadWrapper, 0, 0, 0);
}