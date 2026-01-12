#pragma once
#include "../Engine/InputsEnums.h"
#include "../Engine/InputsMethods.h"
#include <unordered_map>

enum class KeyAction
{
	MoveLeft,
	MoveRight,
	MoveForward,
	MoveBackward,
	Jump,
	Dash,
	Reload,

	InventorySlot1,
	InventorySlot2,
	InventorySlot3,
	InventorySlot4,

	ToggleFullScreen,
};

class KeyBinds
{
	static std::unordered_map<KeyAction, gce::Keyboard::Key> m_keybinds;
	static std::unordered_map<gce::Keyboard::Key, KeyAction> m_actionbinds;
	static void AddKeyBind(KeyAction action, gce::Keyboard::Key key);
public:
	static void InitDefaultKeyBinds();
	static void EditKey(KeyAction action, gce::Keyboard::Key newKey);

	static gce::Keyboard::Key GetKeyBind(KeyAction action);
};

