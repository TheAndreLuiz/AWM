#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_keysyms.h>

#include <gtkmm-4.0/gtkmm.h>

#include <gdk/x11/gdkx.h>

int linesTest = 0; // change

bool running = true;
xcb_connection_t *connection;
xcb_screen_t *screen;
xcb_generic_event_t *event;

static xcb_drawable_t win;
uint32_t masks[3];

struct KeyToFunction {
  xcb_keysym_t key;
  void (*spawnFunctionPointer)(char *, char **);
};

KeyToFunction *hotkeys;

static void closewm() { xcb_disconnect(connection); }

static void spawn(char *program, char **arguments) {
  if (fork() == 0) {
    setsid();
    if (fork() != 0) {
      _exit(0);
    }
    execvp(program, arguments);
    _exit(0);
  }
  wait(NULL);
}

static void closeTest(GtkWidget *widget, gpointer data) {
  printf("Hello World\n");
}

static void spawnTest(GtkWidget *widget, GtkWidget *data) {
  GtkEntryBuffer *entryBuffer1 = gtk_entry_get_buffer((GtkEntry *)data);
  std::string temp = (char *)gtk_entry_buffer_get_text(entryBuffer1);

  std::string program = temp.substr(0, temp.find(" "));
  std::string argumentsString = temp.substr(temp.find(" ") + 1, temp.length());

  char *arguments[std::count(argumentsString.begin(), argumentsString.end(),
                             ' ')]; // change and change everything
  int i = 0;
  std::string delimiter = " ";
  int pos = argumentsString.find(delimiter);

  while (pos != std::string::npos) {
    arguments[i] = (char *)argumentsString.substr(0, pos).data();
    argumentsString.erase(0, pos + 1);
    pos = argumentsString.find(delimiter);
    i++;
  }
  arguments[i] = (char *)argumentsString.data();

  for (int i = 0; i < 2; i++) {
    std::cout << arguments[i] << " <- \n";
  }

  spawn((char *)program.data(), arguments); // change
}

static void create_widgets(GtkApplication *app) {
  GtkWidget *window = gtk_application_window_new(app);

  gtk_window_set_title(GTK_WINDOW(window), "Window");
  GtkWidget *grid = gtk_grid_new();
  gtk_window_set_child(GTK_WINDOW(window), grid);

  GtkWidget *windowEntry1 = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid), windowEntry1, 0, 2, 2, 1);

  GtkWidget *windowEntry2 = gtk_entry_new();
  gtk_grid_attach(GTK_GRID(grid), windowEntry2, 0, 3, 3, 1);

  GtkWidget *spawnWindowButton = gtk_button_new_with_label("Spawn window");
  g_signal_connect(spawnWindowButton, "clicked", G_CALLBACK(spawnTest),
                   windowEntry1);
  gtk_grid_attach(GTK_GRID(grid), spawnWindowButton, 0, 0, 1, 1);

  GtkWidget *button2 = gtk_button_new_with_label("Close window");
  g_signal_connect(button2, "clicked", G_CALLBACK(closeTest), NULL);
  gtk_grid_attach(GTK_GRID(grid), button2, 1, 0, 1, 1);

  GtkWidget *button3 = gtk_button_new_with_label("Quit");
  g_signal_connect_swapped(button3, "clicked", G_CALLBACK(gtk_window_destroy),
                           window);
  gtk_grid_attach(GTK_GRID(grid), button3, 0, 1, 2, 1);

  GdkDisplay *display = gdk_display_get_default();
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, "/home/a/awm/style.css");
  gtk_style_context_add_provider_for_display(
      display, GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);
  g_object_unref(provider);

  gtk_widget_show(window);
  std::cout << "GTK DONE"
            << "\n";
}
void killclient(char **com) { xcb_kill_client(connection, win); }

void updateCssTest(char **com) {
  GdkDisplay *display = gdk_display_get_default();
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, "/home/a/awm/style.css");
  gtk_style_context_add_provider_for_display(
      display, GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);
  g_object_unref(provider);
}

void closewm(xcb_connection_t *connection) { xcb_disconnect(connection); }

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

int setup() {
  connection = xcb_connect(NULL, NULL);

  int connectionResult = xcb_connection_has_error(connection);
  if (connectionResult > 0) {
    std::cout << "xcb_connection_has_error\n";
    return connectionResult;
  }

  screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

  masks[0] = XCB_EVENT_MASK_STRUCTURE_NOTIFY |
             XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
             XCB_EVENT_MASK_PROPERTY_CHANGE;
  xcb_change_window_attributes_checked(connection, screen->root,
                                       XCB_CW_EVENT_MASK, masks);
  xcb_ungrab_key(connection, XCB_GRAB_ANY, screen->root, XCB_MOD_MASK_ANY);

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

  return connectionResult;
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
  if ((window != 0)) {
    xcb_set_input_focus(connection, XCB_INPUT_FOCUS_POINTER_ROOT, window,
                        XCB_CURRENT_TIME);
    xcb_flush(connection);
  }
}

static void handleEnterNotify() {
  xcb_enter_notify_event_t *e = (xcb_enter_notify_event_t *)event;
  setFocus(e->event);
}

static void handleLeaveNotify() {
  xcb_leave_notify_event_t *e = (xcb_leave_notify_event_t *)event;
  setFocus(screen->root);
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
  xcb_configure_window(connection, e->event, XCB_CONFIG_WINDOW_STACK_MODE,
                       masks);
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
  setFocus(win);
  xcb_flush(connection);
}

static void handleKeyPress() {
  xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
  xcb_keysym_t keysym = xcb_get_keysym(connection, e->detail);
  hotkeys[0].spawnFunctionPointer("alacritty", NULL);
}

static void handleCreateNotify() {
  xcb_create_notify_event_t *e = (xcb_create_notify_event_t *)event;
  xcb_map_window(connection, e->parent);
  std::cout << "width: " << e->width << "\n";
  xcb_flush(connection);
}

static void handleConfigureRequest() {
  xcb_configure_request_event_t *e = (xcb_configure_request_event_t *)event;
  xcb_map_window(connection, e->window);
  xcb_flush(connection);
}

static void handleConfigureNotify() {
  xcb_configure_notify_event_t *e = (xcb_configure_notify_event_t *)event;
  xcb_map_window(connection, e->window);
  xcb_flush(connection);
}

void handleEvent() {
  switch (event->response_type) {
  case XCB_CONFIGURE_NOTIFY:
    handleConfigureNotify();
    break;
  case XCB_CONFIGURE_REQUEST:
    handleConfigureRequest();
    break;
  case XCB_CREATE_NOTIFY:
    handleCreateNotify();
    break;
  case XCB_EXPOSE:
    break;
  case XCB_MOTION_NOTIFY:
    handleMotionNotify();
    break;
  case XCB_ENTER_NOTIFY:
    handleEnterNotify();
    break;
  case XCB_LEAVE_NOTIFY:
    handleLeaveNotify();
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
  default:
    break;
  }
}

static void on_map(GtkWidget *widget, gpointer args) {
  GtkWindow *window = GTK_WINDOW(widget);
  GdkDisplay *display = gtk_widget_get_display(GTK_WIDGET(widget));
  GdkSurface *surface;
  Window xframe;
  unsigned long data[4];

  surface = gtk_native_get_surface(GTK_NATIVE(window));
  if (!surface)
    return;

  data[0] = 10;
  data[1] = 10;
  data[2] = 10;
  data[3] = 10;

  xframe = gdk_x11_surface_get_xid(surface);

  gdk_x11_display_error_trap_push(display);

  xcb_map_window(connection, xframe);
  xcb_flush(connection);
}

static void activate(GtkApplication *app, gpointer user_data) {
  create_widgets(app);
}

void xcb_event_loop() {
  GdkEvent *gdkEventTest;
  while (running) {
    event = xcb_wait_for_event(connection);
    handleEvent();
  }
}

void setWallpaper() {
  xcb_pixmap_t pixmap = xcb_generate_id(connection);
  xcb_create_pixmap(connection, screen->root_depth, pixmap, screen->root,
                    screen->width_in_pixels, screen->height_in_pixels);

  xcb_rectangle_t rect = {0, 0, screen->width_in_pixels,
                          screen->height_in_pixels};

  xcb_gcontext_t gc = xcb_generate_id(connection);
  xcb_create_gc(connection, gc, pixmap, 0, 0);

  uint32_t values[1] = {0xcccccc};
  xcb_change_gc(connection, gc, XCB_GC_FOREGROUND, values);
  xcb_poly_fill_rectangle(connection, pixmap, gc, 1, &rect);

  xcb_change_window_attributes(connection, screen->root, XCB_CW_BACK_PIXMAP,
                               &pixmap);
  xcb_flush(connection);
  xcb_clear_area(connection, 0, screen->root, 0, 0, screen->width_in_pixels,
                 screen->height_in_pixels);
  xcb_free_pixmap(connection, pixmap);
}

int main(int argc, char **argv) {
  setup();
  setWallpaper();

  std::thread xcbThread(xcb_event_loop);

  GdkDisplay *display = gdk_display_get_default();

  GtkApplication *app;
  app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

  std::cout << "gtk running app\n";
  g_application_run(G_APPLICATION(app), argc, argv);
  std::cout << "app done\n";
  g_object_unref(app);

  return 0;
}
