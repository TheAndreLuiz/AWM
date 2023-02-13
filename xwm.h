#include <xcb/xcb_keysyms.h>

typedef struct {
  unsigned int mod;
  unsigned int keysym;
  void (*func)(char **com);
  char **com;
} Key;

typedef struct {
  uint32_t request;
  void (*func)();
} handler_func_t;
