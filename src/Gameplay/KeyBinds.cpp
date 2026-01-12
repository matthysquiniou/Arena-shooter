#include "KeyBinds.h"

std::unordered_map<KeyAction, gce::Keyboard::Key> KeyBinds::m_keybinds;
std::unordered_map<gce::Keyboard::Key, KeyAction> KeyBinds::m_actionbinds;

void KeyBinds::AddKeyBind(KeyAction action, gce::Keyboard::Key key)
{
	if (m_keybinds.contains(action) || m_actionbinds.contains(key))
	{
		std::cerr << "You have already assigned this action or key !\n";
		return;
	}

	m_keybinds[action] = key;
	m_actionbinds[key] = action;
}

void KeyBinds::InitDefaultKeyBinds()
{
	AddKeyBind(KeyAction::MoveLeft, gce::Keyboard::Q);
	AddKeyBind(KeyAction::MoveRight, gce::Keyboard::D);
	AddKeyBind(KeyAction::MoveForward, gce::Keyboard::Z);
	AddKeyBind(KeyAction::MoveBackward, gce::Keyboard::S);

	AddKeyBind(KeyAction::Jump, gce::Keyboard::SPACE); // TODO Remove
	AddKeyBind(KeyAction::Dash, gce::Keyboard::LSHIFT);
	AddKeyBind(KeyAction::Reload, gce::Keyboard::R);

	AddKeyBind(KeyAction::InventorySlot1, gce::Keyboard::_1);
	AddKeyBind(KeyAction::InventorySlot2, gce::Keyboard::_2);
	AddKeyBind(KeyAction::InventorySlot3, gce::Keyboard::_3);
	AddKeyBind(KeyAction::InventorySlot4, gce::Keyboard::_4);

	AddKeyBind(KeyAction::ToggleFullScreen, gce::Keyboard::F11);
}

void KeyBinds::EditKey(KeyAction action, gce::Keyboard::Key newKey)
{
	if (m_keybinds.contains(action))
	{
		auto it = m_actionbinds.find(newKey);

		if (it != m_actionbinds.end())
		{
			gce::Keyboard::Key oldKey = m_keybinds[action];
			KeyAction foundAction = it->second;

			m_keybinds[foundAction] = oldKey;
			m_actionbinds[oldKey] = foundAction;
		}
		
		m_keybinds[action] = newKey;
		m_actionbinds[newKey] = action;
	}
}

gce::Keyboard::Key KeyBinds::GetKeyBind(KeyAction action)
{
	if (m_keybinds.contains(action))
	{
		return m_keybinds[action];
	}

	std::cerr << "Error : This key doesn't exists on this action !\n";
	return gce::Keyboard::END;
}
