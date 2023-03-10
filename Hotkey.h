#include <iostream>
#include <xcb/xcb_keysyms.h>

class Hotkey {
public:
  Hotkey(xcb_keysym_t hotkey, void (*function)(const std::string *),
         const std::string *argument);

  void(*getFunction());

  xcb_keysym_t getHotkey();

private:
  xcb_keysym_t hotkey;

  void (*function)(const std::string *);
};
