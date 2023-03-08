#include "Hotkey.h"
#include <map>
#include <xcb/xcb_keysyms.h>

class Hotkeys {
public:
  ~Hotkeys();

  void addHotkey(Hotkey *hotkey);

  void (*hotkeyToFunction(xcb_keysym_t))(const std::string *);

private:
  std::map<xcb_keysym_t, void (*)(const std::string *)> hotkeyToFunctionMap;
};
