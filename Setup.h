#include "Hotkeys.h"
#include "XServerComunicator.h"

class Setup {
public:
  void setup();

private:
  XServerComunicator *setupXServerComunicator();
  Hotkeys *setupHotkeys();
};
