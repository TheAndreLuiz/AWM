#include <xcb/xcb.h>

class XServerComunicator {
public:
  XServerComunicator();

  ~XServerComunicator();

  xcb_connection_t *getConnection();

  xcb_screen_t *getScreen();

private:
  xcb_connection_t *connection;

  xcb_screen_t *screen;
};
