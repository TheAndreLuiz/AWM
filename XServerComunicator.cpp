#include "XServerComunicator.h"
#include <iostream>
#include <xcb/xcb.h>

XServerComunicator::XServerComunicator() {
  this->connection = xcb_connect(NULL, NULL);
  this->screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
}

XServerComunicator::~XServerComunicator() {
  std::cout << "TODO: implement XServerComunicator destructor\n";
}
