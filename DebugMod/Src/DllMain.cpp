#include <windows.h>

#include <Escort.h>

#include <kenshi/Kenshi.h>
#include <kenshi/GameWorld.h>
#include <kenshi/ModInfo.h>
#include <kenshi/Faction.h>
#include <kenshi/PlayerInterface.h>

#include <_IncompleteTypes.h>

#include <core/Scanner.h>

void (*Ownerships_addMoney_orig)(Kenshi::Ownerships* ownerships, int32_t price);

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


void DoLog(const char* format, ...) {
    char buffer[1024]; // temporary buffer for formatted string

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    std::cout << buffer << std::endl;
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

void addMoneyHook(Kenshi::Ownerships* ownership, int32_t price)
{
    DoLog("[DebugMod] In the money hook");

	Kenshi::GameWorld* gameWorld = &Kenshi::GetGameWorld();
    auto player = gameWorld->player;
	auto platoon = player->currentPlatoon;
	auto activePlatoon = platoon->currentActivePlatoon;
    
    for (int i = 0; i < gameWorld->player->currentPlatoon->currentActivePlatoon->platoonMemberCount; i++)
        DoLog("\t%s", gameWorld->player->currentPlatoon->currentActivePlatoon->platoonMembers->data[i].character->displayName);

    Ownerships_addMoney_orig(ownership, price * 2);
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
    DoLog("[DebugMod] Initialized. PID: %d. GameWorld address: 0x%p", GetCurrentProcessId(), &Kenshi::GetGameWorld());

    validateOffset("GameDataContainer::gamedataID", offsetof(Kenshi::GameDataContainer, gamedataID), 0x50);
    validateOffset("GameWorld::player", offsetof(Kenshi::GameWorld, player), 0x580);


    validateOffset("PlayerInterface::selectBox", offsetof(Kenshi::PlayerInterface, selectBox), 0xA8);
    validateOffset("PlayerInterface::moveMarker", offsetof(Kenshi::PlayerInterface, moveMarker), 0xE8);
    validateOffset("PlayerInterface::selectedCharacter", offsetof(Kenshi::PlayerInterface, selectedCharacter), 0xF0);
    validateOffset("PlayerInterface::townsActive", offsetof(Kenshi::PlayerInterface, townsActive), 0x160);
    validateOffset("PlayerInterface::mRightDown", offsetof(Kenshi::PlayerInterface, mRightDown), 0x2F4);

    validateOffset("Character::naturalWeapon", offsetof(Kenshi::Character, naturalWeapon), 0x6D0);

    validateOffset("SelectionBox::startPos", offsetof(Kenshi::SelectionBox, startPos), 0x00);
    validateOffset("SelectionBox::volume", offsetof(Kenshi::SelectionBox, volume), 0x08);
    validateOffset("SelectionBox::widget", offsetof(Kenshi::SelectionBox, widget), 0x30);

    const char* purchase_item_pattern = "\x8B\x81\x88\x00\x00\x00\x3B\xC2\x7D\x07\x85\xD2\x78\x03\x32\xC0";
    const char* purchase_item_mask = "xxx???xxxxxxxxxx";

    uintptr_t moduleBase;
    size_t moduleSize;
    GetModuleCodeRegion(gameVersion.GetBinaryName().c_str(), &moduleBase, &moduleSize);

    uintptr_t targetOffset = PatternScan(moduleBase, moduleSize, purchase_item_pattern, purchase_item_mask);
    uintptr_t targetAddress = moduleBase + targetOffset;

    Ownerships_addMoney_orig = Escort::JmpReplaceHook<void(Kenshi::Ownerships*, int32_t)>((void*)targetAddress, addMoneyHook, 6);

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