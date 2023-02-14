#include "xwm.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>

const static bool running = true;
xcb_connection_t *connection;
xcb_screen_t *screen;
xcb_generic_event_t *event;

static char *termcmd[] = {"konsole", NULL};
static char *menucmd[] = {"rofi", "-show drun"};

static xcb_drawable_t win;
uint32_t masks[3];

#define aKey 0x0061
#define bKey 0x0062
#define cKey 0x0063
#define dKey 0x0064
#define eKey 0x0065
#define fKey 0x0066
#define gKey 0x0067
#define hKey 0x0068
#define iKey 0x0069
#define jKey 0x006a
#define kKey 0x006b
#define lKey 0x006c
#define mKey 0x006d
#define nKey 0x006e
#define oKey 0x006f
#define pKey 0x0070
#define qKey 0x0071
#define rKey 0x0072
#define sKey 0x0073
#define tKey 0x0074
#define uKey 0x0075
#define vKey 0x0076
#define wKey 0x0077
#define xKey 0x0078
#define yKey 0x0079
#define zKey 0x007a
#define spaceKey 0x0020
#define enterKey 0xff0d

static void closewm() { xcb_disconnect(connection); }

static void spawn(char **com) {
  if (fork() == 0) {
    setsid();
    if (fork() != 0) {
      _exit(0);
    }
    execvp((char *)com[0], (char **)com);
    _exit(0);
  }
  wait(NULL);
}

void killclient(char **com) { xcb_kill_client(connection, win); }

void closewm(xcb_connection_t *connection) { xcb_disconnect(connection); }

static Key hotkeys[] = {{XCB_MOD_MASK_4, enterKey, spawn, termcmd},
                        {XCB_MOD_MASK_4, spaceKey, spawn, menucmd},
                        {XCB_MOD_MASK_4, qKey, killclient, NULL}};

static xcb_keycode_t *xcb_get_keycodes(xcb_connection_t *connection,
                                       xcb_keysym_t keysym) {
  xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(connection);
  xcb_keycode_t *keycode;
  keycode = (!(keysyms) ? NULL : xcb_key_symbols_get_keycode(keysyms, keysym));
  xcb_key_symbols_free(keysyms);
  return keycode;
}

static xcb_keysym_t xcb_get_keysym(xcb_connection_t *connection,
                                   xcb_keycode_t keycode) {
  xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(connection);
  xcb_keysym_t keysym;
  keysym = (!(keysyms) ? 0 : xcb_key_symbols_get_keysym(keysyms, keycode, 0));
  xcb_key_symbols_free(keysyms);
  return keysym;
}

void setup(xcb_connection_t *connection) {
  masks[0] =
      XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
      XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_PROPERTY_CHANGE;
  xcb_change_window_attributes_checked(connection, screen->root,
                                       XCB_CW_EVENT_MASK, masks);
  xcb_ungrab_key(connection, XCB_GRAB_ANY, screen->root, XCB_MOD_MASK_ANY);
  int key_table_size = sizeof(hotkeys) / sizeof(*hotkeys);
  for (int i = 0; i < key_table_size; ++i) { // god, change pls
    xcb_keycode_t *keycode = xcb_get_keycodes(connection, hotkeys[i].keysym);
    if (keycode != NULL) {
      xcb_grab_key(connection, 1, screen->root, hotkeys[i].mod, *keycode,
                   XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    }
  }
  xcb_flush(connection);
  xcb_grab_button(connection, 0, screen->root,
                  XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
                  XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, screen->root,
                  XCB_NONE, 1, XCB_MOD_MASK_4);
  xcb_grab_button(connection, 0, screen->root,
                  XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
                  XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, screen->root,
                  XCB_NONE, 3, XCB_MOD_MASK_4);
  xcb_flush(connection);
}

static void handleMotionNotify() {
  xcb_query_pointer_cookie_t coord =
      xcb_query_pointer(connection, screen->root);
  xcb_query_pointer_reply_t *poin =
      xcb_query_pointer_reply(connection, coord, 0);
  if ((masks[2] == (uint32_t)(1)) && (win != 0)) {
    xcb_get_geometry_cookie_t geom_now = xcb_get_geometry(connection, win);
    xcb_get_geometry_reply_t *geom =
        xcb_get_geometry_reply(connection, geom_now, NULL);
    uint16_t geom_x = geom->width;
    uint16_t geom_y = geom->height;
    masks[0] = ((poin->root_x + geom_x / 2) > screen->width_in_pixels)
                   ? (screen->width_in_pixels - geom_x)
                   : poin->root_x - geom_x / 2;
    masks[1] = ((poin->root_y + geom_y / 2) > screen->height_in_pixels)
                   ? (screen->height_in_pixels - geom_y)
                   : poin->root_y - geom_y / 2;
    xcb_configure_window(connection, win,
                         XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, masks);
  } else if ((masks[2] == (uint32_t)(3)) && (win != 0)) {
    xcb_get_geometry_cookie_t geom_now = xcb_get_geometry(connection, win);
    xcb_get_geometry_reply_t *geom =
        xcb_get_geometry_reply(connection, geom_now, NULL);
    if (!((poin->root_x <= geom->x) || (poin->root_y <= geom->y))) {
      masks[0] = poin->root_x - geom->x;
      masks[1] = poin->root_y - geom->y;
      if ((masks[0] >= (uint32_t)(100)) &&
          (masks[1] >= (uint32_t)(100))) { // change magic values
        xcb_configure_window(connection, win,
                             XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                             masks);
      }
    }
  }
}

static void setFocus(xcb_drawable_t window) {
  if ((window != 0) && (window != screen->root)) {
    xcb_set_input_focus(connection, XCB_INPUT_FOCUS_POINTER_ROOT, window,
                        XCB_CURRENT_TIME);
  }
}

static void handleKeyPress() {
  xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
  xcb_keysym_t keysym = xcb_get_keysym(connection, e->detail);
  win = e->child;
  int key_table_size = sizeof(hotkeys) / sizeof(*hotkeys);
  for (int i = 0; i < key_table_size; ++i) {
    if ((hotkeys[i].keysym == keysym) && (hotkeys[i].mod == e->state)) {
      hotkeys[i].func(hotkeys[i].com);
    }
  }
}

static void handleEnterNotify() {
  xcb_enter_notify_event_t *e = (xcb_enter_notify_event_t *)event;
  setFocus(e->event);
}

static void handleButtonRelease() {
  xcb_ungrab_pointer(connection, XCB_CURRENT_TIME);
}

static void handleDestroyNotify() {
  xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *)event;
  xcb_kill_client(connection, e->window);
}

static void handleFocusIn() {
  xcb_focus_in_event_t *e = (xcb_focus_in_event_t *)event;
  masks[0] = XCB_STACK_MODE_ABOVE;
  xcb_configure_window(connection, e->event, XCB_CONFIG_WINDOW_STACK_MODE, masks);
  xcb_flush(connection);
}

static void handleFocusOut() {
  xcb_focus_out_event_t *e = (xcb_focus_out_event_t *)event;
}

static void handleMapRequest() {
  xcb_map_request_event_t *e = (xcb_map_request_event_t *)event;

  xcb_get_geometry_cookie_t cookie = xcb_get_geometry(connection, e->window);
  xcb_get_geometry_reply_t *reply =
      xcb_get_geometry_reply(connection, cookie, NULL);
  free(reply);

  xcb_map_window(connection, e->window);

  uint32_t vals[5];
  vals[0] = (screen->width_in_pixels / 2) - (reply->width / 2);
  vals[1] = (screen->height_in_pixels / 2) - (reply->height / 2);
  vals[2] = reply->width;
  vals[3] = reply->height;
  vals[4] = 0;

  xcb_configure_window(connection, e->window,
                       XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                           XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT |
                           XCB_CONFIG_WINDOW_BORDER_WIDTH,
                       vals);
  xcb_flush(connection);
  masks[0] = XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_FOCUS_CHANGE;
  xcb_change_window_attributes_checked(connection, e->window, XCB_CW_EVENT_MASK,
                                       masks);
  setFocus(e->window);
}

static void handleButtonPress() {
  xcb_button_press_event_t *e = (xcb_button_press_event_t *)event;
  win = e->child;
  xcb_grab_pointer(connection, false, screen->root,
                   XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                       XCB_EVENT_MASK_POINTER_MOTION,
                   XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE, XCB_NONE,
                   XCB_CURRENT_TIME);
  masks[0] = XCB_STACK_MODE_ABOVE;
  xcb_configure_window(connection, win, XCB_CONFIG_WINDOW_STACK_MODE, masks);
  masks[2] = ((1 == e->detail) ? 1 : ((win != 0) ? 3 : 0));
  xcb_flush(connection);
}

void handleEvent() {
  switch (event->response_type) {
  case XCB_MOTION_NOTIFY:
    handleMotionNotify();
    break;
  case XCB_ENTER_NOTIFY:
    handleEnterNotify();
    break;
  case XCB_DESTROY_NOTIFY:
    handleDestroyNotify();
    break;
  case XCB_BUTTON_PRESS:
    handleButtonPress();
    break;
  case XCB_BUTTON_RELEASE:
    handleButtonRelease();
    break;
  case XCB_KEY_PRESS:
    handleKeyPress();
    break;
  case XCB_MAP_REQUEST:
    handleMapRequest();
    break;
  case XCB_FOCUS_IN:
    handleFocusIn();
    break;
  case XCB_FOCUS_OUT:
    handleFocusOut();
    break;
  case XCB_NONE:
    break;
  }
}

int main(int argc, char *argv[]) {
  connection = xcb_connect(NULL, NULL);

  int connectionResult = xcb_connection_has_error(connection);
  if (connectionResult > 0) {
    printf("xcb_connection_has_error\n");
    return connectionResult;
  }

  screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

  xcb_pixmap_t pixmap = xcb_generate_id(connection);
  xcb_create_pixmap(connection, screen->root_depth, pixmap, screen->root,
                    screen->width_in_pixels, screen->height_in_pixels);

  xcb_rectangle_t rect = {0, 0, screen->width_in_pixels,
                          screen->height_in_pixels};

  xcb_gcontext_t gc = xcb_generate_id(connection);
  xcb_create_gc(connection, gc, pixmap, 0, 0);

  uint32_t values[1] = {screen->white_pixel};
  xcb_change_gc(connection, gc, XCB_GC_FOREGROUND, values);
  xcb_poly_fill_rectangle(connection, pixmap, gc, 1, &rect);

  xcb_change_window_attributes(connection, screen->root, XCB_CW_BACK_PIXMAP,
                               &pixmap);
  xcb_flush(connection);
  xcb_clear_area(connection, 0, screen->root, 0, 0, screen->width_in_pixels,
                 screen->height_in_pixels);
  xcb_free_pixmap(connection, pixmap);

  setup(connection);

  while (running) {
    event = xcb_wait_for_event(connection);
    handleEvent();
  }

  return connectionResult;
}
