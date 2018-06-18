#include <comfybar/bar.h>
#include <iostream>
#include <cstring>
#include <X11/Xatom.h>

namespace comfybar {
  Bar::Bar(int argc, char** argv):m_created(false), m_width(100),
    m_height(30) {
  }

  void Bar::create() {
    try {
      m_config.loadFromFile("res/config.conf");
      // the config is loaded ==> get all needed values
      char* temp_value;
      temp_value = m_config.getSectionValue("bar", "height");
      if(temp_value) {
        m_height = atoi(temp_value);
        //printf("Value: %d\n", m_height);
      }

      temp_value = m_config.getSectionValue("bar", "bottom");
      if(temp_value) {
        m_bottom = (strcmp(temp_value, "true") == 0)? true : false;
      }

    } catch(ConfigException& ex) {
      if(!ex.getLineNumber()) {
        std::cout << ex.getMessage() << std::endl;
      } else {
        ex.printMessage();
        return;
      }
    }

    m_display = XOpenDisplay(None);
    if(!m_display) {
      fprintf(stderr, "Unable to connect to the X server.\n");
      return;
    }
    m_screen = XDefaultScreen(m_display);

    uint mask = CWOverrideRedirect | CWBackPixel | CWEventMask;
    m_wattributes.override_redirect = 0;
    m_wattributes.background_pixel = 0x2e3440;
    m_wattributes.event_mask = ExposureMask | ButtonPressMask;

    m_window = XCreateWindow(m_display, XRootWindow(m_display, m_screen),
      0, m_bottom?XDisplayHeight(m_display, m_screen) : 0, 1, m_height, 0, 
        XDefaultDepth(m_display, m_screen), InputOutput, 
          XDefaultVisual(m_display, m_screen), 
            mask, &m_wattributes);
    enum {
      NET_WM_WINDOW_TYPE,
      NET_WM_WINDOW_TYPE_DOCK,
      NET_WM_DESKTOP,
      NET_WM_STRUT_PARTIAL,
      NET_WM_STRUT,
      NET_WM_STATE,
      NET_WM_STATE_STICKY,
      NET_WM_STATE_ABOVE,
    };
    const char *atom_names[] = {
        "_NET_WM_WINDOW_TYPE",
        "_NET_WM_WINDOW_TYPE_DOCK",
        "_NET_WM_DESKTOP",
        "_NET_WM_STRUT_PARTIAL",
        "_NET_WM_STRUT",
        "_NET_WM_STATE",
        "_NET_WM_STATE_STICKY",
        "_NET_WM_STATE_ABOVE",
    };
    const int atoms_count = sizeof(atom_names)/sizeof(char*);
    Atom atom_list[atoms_count];

    for(int i = 0; i < atoms_count; i++) {
      atom_list[i] = XInternAtom(m_display, atom_names[i], 0);
    }

    // set all needed properties for the bar
    XChangeProperty(m_display, m_window, atom_list[NET_WM_WINDOW_TYPE], XA_ATOM,
      32, PropModeReplace, (unsigned char*)&atom_list[NET_WM_WINDOW_TYPE_DOCK], 
        1);
    XChangeProperty(m_display, m_window, atom_list[NET_WM_STATE], XA_ATOM,
      32, PropModeAppend, (unsigned char*)&atom_list[NET_WM_STATE_STICKY], 
        2);
    XChangeProperty(m_display, m_window, atom_list[NET_WM_DESKTOP], 
      XA_CARDINAL, 32, PropModeReplace, 
        (unsigned char*)(const int[]) {-1}, 
          1);
    XChangeProperty(m_display, m_window, XInternAtom(m_display, "WM_NAME", 0), 
      XA_STRING, 8, PropModeReplace, 
        (unsigned char*)"comfybar", 
          strlen("comfybar") + 1);

    // bar is created with no errors ==> show it
    m_created = true;
  }
  
  bool Bar::isCreated() const {
    return m_created;
  }

  void Bar::m_handleMouseButtonPress(XEvent* event) {
    m_running = false;
  }
  
  void Bar::m_handleExposureEvent(XEvent* event) {
  }

  void Bar::show() {
    XEvent event;
    m_running = true; // set running to true

    XMapWindow(m_display, m_window);
    while(m_running) {
      XNextEvent(m_display, &event);
      switch(event.type) {
        case Expose:
          m_handleExposureEvent(&event);
          break;
        case ButtonPress:
          m_handleMouseButtonPress(&event);
          break;
      }
    }
  }

  Bar::~Bar() {
    if(m_display) {
      XDestroyWindow(m_display, m_window);
      XCloseDisplay(m_display);
    }
  }
}