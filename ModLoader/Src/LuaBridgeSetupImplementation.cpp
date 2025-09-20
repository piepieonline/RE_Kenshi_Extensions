/***
    THIS FILE IS AUTO-GENERATED FROM KENSHILIB. RUNNING A CMAKE BUILD WILL REPLACE IT.
    ...in theory, not done yet...
***/


#include "../Include/LuaBridgeSetup.h";

#include "LuaBridge/LuaBridge.h";

#include <mygui/MyGUI.h>

#include <kenshi/util/hand.h>
#include <kenshi/Kenshi.h>
#include <kenshi/GameWorld.h>
#include <kenshi/Item.h>
#include <kenshi/ModInfo.h>
#include <kenshi/Faction.h>
#include <kenshi/PlayerInterface.h>

#include <_IncompleteTypes.h>

#include "../Include/ModLoader.h"

void CreateLuaClasses(lua_State* L)
{
    luabridge::getGlobalNamespace(L)
        .beginNamespace("Kenshi")

            .beginClass<Kenshi::InventoryIcon>("InventoryIcon")
            .addStaticFunction("CastTo", +[](uintptr_t ptr) -> Kenshi::InventoryIcon* { return reinterpret_cast<Kenshi::InventoryIcon*>(ptr); }) // TODO: Script add this to every class
                .addProperty("inventoryItem", +[](const Kenshi::InventoryIcon* self) { return self->inventoryItem; })
                .addProperty("widget", +[](const Kenshi::InventoryIcon* self) { return self->widget; })
            .endClass()

            .beginClass<Kenshi::Item>("Item")
                .addProperty("displayName", +[](const Kenshi::Item* item) { return std::string(item->displayName); }) // TODO: Is this the best way? It means a specific accessor for all strings
                .addProperty("itemWidth", &Kenshi::Item::itemWidth)
                .addProperty("itemHeight", &Kenshi::Item::itemHeight)
                .addProperty("data", +[](const Kenshi::Item* self) { return self->data; })
                .addProperty("handle", +[](const Kenshi::Item* self) { return self->handle; })
            .endClass()

            .beginClass<Kenshi::hand>("hand")
                .addProperty("type", &Kenshi::hand::type)
                .addProperty("container", &Kenshi::hand::container)
                .addProperty("containerSerial", &Kenshi::hand::containerSerial)
                .addProperty("index", &Kenshi::hand::index)
                .addProperty("serial", &Kenshi::hand::serial)
            .endClass()

            .beginClass<Kenshi::GameData>("GameData")
                .addProperty("id", &Kenshi::GameData::id, false)
            .endClass()

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

        .endNamespace()

        .beginNamespace("MyGUI")
            
            .beginClass<MyGUI::Widget>("Widget")
                .addFunction("setSize", static_cast<void (MyGUI::Widget::*)(int, int)>(&MyGUI::Widget::setSize))
                .addFunction("setRealSize", static_cast<void (MyGUI::Widget::*)(float, float)>(&MyGUI::Widget::setRealSize))
                .addFunction("setRealPosition", static_cast<void (MyGUI::Widget::*)(float, float)>(&MyGUI::Widget::setRealPosition))
                .addFunction("getWidth", &MyGUI::Widget::getWidth)
                .addFunction("getHeight", &MyGUI::Widget::getHeight)
                .addFunction("getChildCount", &MyGUI::Widget::getChildCount)
                .addFunction("getChildAt", &MyGUI::Widget::getChildAt)
            .endClass()

            .beginClass<MyGUI::RotatingSkin>("RotatingSkin")
                .addStaticFunction("FromWidget", +[](MyGUI::Widget* widget) { return KenshiLib_Wrappers::MyGUI_Helpers::GetRotatingSkin(widget); })
                .addFunction("setCenter", +[](MyGUI::RotatingSkin* skin, int x, int y) { skin->setCenter(MyGUI::IntPoint(x, y)); })
                .addFunction("setAngle", &MyGUI::RotatingSkin::setAngle)
            .endClass()

        .endNamespace()
    ;
}

void CreateLuaEnums(lua_State* L)
{
    // Macro to make enum table creation easier
#define ADD_ENUM_VALUE(name, value) \
    lua_pushinteger(L, value); \
    lua_setfield(L, -2, #name); \
    lua_pushstring(L, #name); \
    lua_seti(L, -2, value);  

#pragma region OISKeyCode_Definitions

    lua_newtable(L);

    // Fill the table
    ADD_ENUM_VALUE(KC_UNASSIGNED, 0x00);
    ADD_ENUM_VALUE(KC_ESCAPE, 0x01);
    ADD_ENUM_VALUE(KC_1, 0x02);
    ADD_ENUM_VALUE(KC_2, 0x03);
    ADD_ENUM_VALUE(KC_3, 0x04);
    ADD_ENUM_VALUE(KC_4, 0x05);
    ADD_ENUM_VALUE(KC_5, 0x06);
    ADD_ENUM_VALUE(KC_6, 0x07);
    ADD_ENUM_VALUE(KC_7, 0x08);
    ADD_ENUM_VALUE(KC_8, 0x09);
    ADD_ENUM_VALUE(KC_9, 0x0A);
    ADD_ENUM_VALUE(KC_0, 0x0B);
    ADD_ENUM_VALUE(KC_MINUS, 0x0C);
    ADD_ENUM_VALUE(KC_EQUALS, 0x0D);
    ADD_ENUM_VALUE(KC_BACK, 0x0E);
    ADD_ENUM_VALUE(KC_TAB, 0x0F);
    ADD_ENUM_VALUE(KC_Q, 0x10);
    ADD_ENUM_VALUE(KC_W, 0x11);
    ADD_ENUM_VALUE(KC_E, 0x12);
    ADD_ENUM_VALUE(KC_R, 0x13);
    ADD_ENUM_VALUE(KC_T, 0x14);
    ADD_ENUM_VALUE(KC_Y, 0x15);
    ADD_ENUM_VALUE(KC_U, 0x16);
    ADD_ENUM_VALUE(KC_I, 0x17);
    ADD_ENUM_VALUE(KC_O, 0x18);
    ADD_ENUM_VALUE(KC_P, 0x19);
    ADD_ENUM_VALUE(KC_LBRACKET, 0x1A);
    ADD_ENUM_VALUE(KC_RBRACKET, 0x1B);
    ADD_ENUM_VALUE(KC_RETURN, 0x1C);
    ADD_ENUM_VALUE(KC_LCONTROL, 0x1D);
    ADD_ENUM_VALUE(KC_A, 0x1E);
    ADD_ENUM_VALUE(KC_S, 0x1F);
    ADD_ENUM_VALUE(KC_D, 0x20);
    ADD_ENUM_VALUE(KC_F, 0x21);
    ADD_ENUM_VALUE(KC_G, 0x22);
    ADD_ENUM_VALUE(KC_H, 0x23);
    ADD_ENUM_VALUE(KC_J, 0x24);
    ADD_ENUM_VALUE(KC_K, 0x25);
    ADD_ENUM_VALUE(KC_L, 0x26);
    ADD_ENUM_VALUE(KC_SEMICOLON, 0x27);
    ADD_ENUM_VALUE(KC_APOSTROPHE, 0x28);
    ADD_ENUM_VALUE(KC_GRAVE, 0x29);
    ADD_ENUM_VALUE(KC_LSHIFT, 0x2A);
    ADD_ENUM_VALUE(KC_BACKSLASH, 0x2B);
    ADD_ENUM_VALUE(KC_Z, 0x2C);
    ADD_ENUM_VALUE(KC_X, 0x2D);
    ADD_ENUM_VALUE(KC_C, 0x2E);
    ADD_ENUM_VALUE(KC_V, 0x2F);
    ADD_ENUM_VALUE(KC_B, 0x30);
    ADD_ENUM_VALUE(KC_N, 0x31);
    ADD_ENUM_VALUE(KC_M, 0x32);
    ADD_ENUM_VALUE(KC_COMMA, 0x33);
    ADD_ENUM_VALUE(KC_PERIOD, 0x34);
    ADD_ENUM_VALUE(KC_SLASH, 0x35);
    ADD_ENUM_VALUE(KC_RSHIFT, 0x36);
    ADD_ENUM_VALUE(KC_MULTIPLY, 0x37);
    ADD_ENUM_VALUE(KC_LMENU, 0x38);
    ADD_ENUM_VALUE(KC_SPACE, 0x39);
    ADD_ENUM_VALUE(KC_CAPITAL, 0x3A);
    ADD_ENUM_VALUE(KC_F1, 0x3B);
    ADD_ENUM_VALUE(KC_F2, 0x3C);
    ADD_ENUM_VALUE(KC_F3, 0x3D);
    ADD_ENUM_VALUE(KC_F4, 0x3E);
    ADD_ENUM_VALUE(KC_F5, 0x3F);
    ADD_ENUM_VALUE(KC_F6, 0x40);
    ADD_ENUM_VALUE(KC_F7, 0x41);
    ADD_ENUM_VALUE(KC_F8, 0x42);
    ADD_ENUM_VALUE(KC_F9, 0x43);
    ADD_ENUM_VALUE(KC_F10, 0x44);
    ADD_ENUM_VALUE(KC_NUMLOCK, 0x45);
    ADD_ENUM_VALUE(KC_SCROLL, 0x46);
    ADD_ENUM_VALUE(KC_NUMPAD7, 0x47);
    ADD_ENUM_VALUE(KC_NUMPAD8, 0x48);
    ADD_ENUM_VALUE(KC_NUMPAD9, 0x49);
    ADD_ENUM_VALUE(KC_SUBTRACT, 0x4A);
    ADD_ENUM_VALUE(KC_NUMPAD4, 0x4B);
    ADD_ENUM_VALUE(KC_NUMPAD5, 0x4C);
    ADD_ENUM_VALUE(KC_NUMPAD6, 0x4D);
    ADD_ENUM_VALUE(KC_ADD, 0x4E);
    ADD_ENUM_VALUE(KC_NUMPAD1, 0x4F);
    ADD_ENUM_VALUE(KC_NUMPAD2, 0x50);
    ADD_ENUM_VALUE(KC_NUMPAD3, 0x51);
    ADD_ENUM_VALUE(KC_NUMPAD0, 0x52);
    ADD_ENUM_VALUE(KC_DECIMAL, 0x53);
    ADD_ENUM_VALUE(KC_OEM_102, 0x56);
    ADD_ENUM_VALUE(KC_F11, 0x57);
    ADD_ENUM_VALUE(KC_F12, 0x58);
    ADD_ENUM_VALUE(KC_F13, 0x64);
    ADD_ENUM_VALUE(KC_F14, 0x65);
    ADD_ENUM_VALUE(KC_F15, 0x66);
    ADD_ENUM_VALUE(KC_KANA, 0x70);
    ADD_ENUM_VALUE(KC_ABNT_C1, 0x73);
    ADD_ENUM_VALUE(KC_CONVERT, 0x79);
    ADD_ENUM_VALUE(KC_NOCONVERT, 0x7B);
    ADD_ENUM_VALUE(KC_YEN, 0x7D);
    ADD_ENUM_VALUE(KC_ABNT_C2, 0x7E);
    ADD_ENUM_VALUE(KC_NUMPADEQUALS, 0x8D);
    ADD_ENUM_VALUE(KC_PREVTRACK, 0x90);
    ADD_ENUM_VALUE(KC_AT, 0x91);
    ADD_ENUM_VALUE(KC_COLON, 0x92);
    ADD_ENUM_VALUE(KC_UNDERLINE, 0x93);
    ADD_ENUM_VALUE(KC_KANJI, 0x94);
    ADD_ENUM_VALUE(KC_STOP, 0x95);
    ADD_ENUM_VALUE(KC_AX, 0x96);
    ADD_ENUM_VALUE(KC_UNLABELED, 0x97);
    ADD_ENUM_VALUE(KC_NEXTTRACK, 0x99);
    ADD_ENUM_VALUE(KC_NUMPADENTER, 0x9C);
    ADD_ENUM_VALUE(KC_RCONTROL, 0x9D);
    ADD_ENUM_VALUE(KC_MUTE, 0xA0);
    ADD_ENUM_VALUE(KC_CALCULATOR, 0xA1);
    ADD_ENUM_VALUE(KC_PLAYPAUSE, 0xA2);
    ADD_ENUM_VALUE(KC_MEDIASTOP, 0xA4);
    ADD_ENUM_VALUE(KC_VOLUMEDOWN, 0xAE);
    ADD_ENUM_VALUE(KC_VOLUMEUP, 0xB0);
    ADD_ENUM_VALUE(KC_WEBHOME, 0xB2);
    ADD_ENUM_VALUE(KC_NUMPADCOMMA, 0xB3);
    ADD_ENUM_VALUE(KC_DIVIDE, 0xB5);
    ADD_ENUM_VALUE(KC_SYSRQ, 0xB7);
    ADD_ENUM_VALUE(KC_RMENU, 0xB8);
    ADD_ENUM_VALUE(KC_PAUSE, 0xC5);
    ADD_ENUM_VALUE(KC_HOME, 0xC7);
    ADD_ENUM_VALUE(KC_UP, 0xC8);
    ADD_ENUM_VALUE(KC_PGUP, 0xC9);
    ADD_ENUM_VALUE(KC_LEFT, 0xCB);
    ADD_ENUM_VALUE(KC_RIGHT, 0xCD);
    ADD_ENUM_VALUE(KC_END, 0xCF);
    ADD_ENUM_VALUE(KC_DOWN, 0xD0);
    ADD_ENUM_VALUE(KC_PGDOWN, 0xD1);
    ADD_ENUM_VALUE(KC_INSERT, 0xD2);
    ADD_ENUM_VALUE(KC_DELETE, 0xD3);
    ADD_ENUM_VALUE(KC_LWIN, 0xDB);
    ADD_ENUM_VALUE(KC_RWIN, 0xDC);
    ADD_ENUM_VALUE(KC_APPS, 0xDD);
    ADD_ENUM_VALUE(KC_POWER, 0xDE);
    ADD_ENUM_VALUE(KC_SLEEP, 0xDF);
    ADD_ENUM_VALUE(KC_WAKE, 0xE3);
    ADD_ENUM_VALUE(KC_WEBSEARCH, 0xE5);
    ADD_ENUM_VALUE(KC_WEBFAVORITES, 0xE6);
    ADD_ENUM_VALUE(KC_WEBREFRESH, 0xE7);
    ADD_ENUM_VALUE(KC_WEBSTOP, 0xE8);
    ADD_ENUM_VALUE(KC_WEBFORWARD, 0xE9);
    ADD_ENUM_VALUE(KC_WEBBACK, 0xEA);
    ADD_ENUM_VALUE(KC_MYCOMPUTER, 0xEB);
    ADD_ENUM_VALUE(KC_MAIL, 0xEC);
    ADD_ENUM_VALUE(KC_MEDIASELECT, 0xED);

    lua_setglobal(L, "OISKeyCode");

#pragma endregion

#undef ADD_ENUM_VALUE
}

void LuaBridgeSetup::CreateLuaStateInternal(lua_State* L)
{
    CreateLuaClasses(L);
    CreateLuaEnums(L);
}