#include "Setup.h"
#include "Spawn.h"

XServerComunicator *setupXServerComunicator() { return new XServerComunicator; }

Hotkeys *Setup::setupHotkeys() {
  std::cout << "TODO: improve imports pragma once and just whats needed\n";
  std::cout << "TODO: header with keys\n";

  Hotkeys *hotkeys = new Hotkeys();

  const std::string test = "a";
  Hotkey hotkeyEnter((xcb_keysym_t)0xff0d, (Spawn::spawn), (&test));

  hotkeys->addHotkey(hotkeyEnter);

  return hotkeys;
}
