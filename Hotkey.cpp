#include "Hotkey.h"

Hotkey::Hotkey(xcb_keysym_t hotkey, void (*function)(const std::string *),
               const std::string *argument) { // change to :
  this->hotkey = hotkey;
  this->function = function;
}
