#include "Hotkeys.h"
#include <iostream>
#include <map>

void Hotkeys::addHotkey(Hotkey *hotkey) {}

void (*Hotkeys::hotkeyToFunction(xcb_keysym_t hotkey))(const std::string *) {
  return this->hotkeyToFunctionMap.find(hotkey)->second;
}
