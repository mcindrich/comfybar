#include <comfybar/bar.h>
#include <iostream>
#include <cstring>

namespace comfybar {
  Bar::Bar(int argc, char** argv):m_created(false), m_width(100),
    m_height(30) {
    
  }

  static void inline barTestCookie(xcb_void_cookie_t cookie, 
    xcb_connection_t* connection, char* msg) {
    xcb_generic_error_t *error = xcb_request_check (connection, cookie);
    if(error) {
      printf("%s\n", msg);
    }
  }
  void Bar::create() {
    try {
      m_config.loadFromFile("res/config.conf");
      char* value = m_config.getSectionValue("bar", "height");
    } catch(ConfigException& ex) {
      if(!ex.getLineNumber()) {
        std::cout << ex.getMessage() << std::endl;
      } else {
        ex.printMessage();
        return;
      }
    }
    
    m_connection = xcb_connect(nullptr, NULL);
    m_screen = xcb_setup_roots_iterator(xcb_get_setup(m_connection)).data;
    m_window = xcb_generate_id(m_connection);
    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_OVERRIDE_REDIRECT | 
      XCB_CW_EVENT_MASK;
    
    // color taken from VSCode Nord Theme base color

    uint32_t values[] { 0x2e3440, 0, XCB_EVENT_MASK_EXPOSURE | 
      XCB_EVENT_MASK_BUTTON_PRESS | 
      XCB_EVENT_MASK_EXPOSURE |
      XCB_EVENT_MASK_BUTTON_MOTION};

    xcb_create_window(m_connection, 0, m_window, 
      m_screen->root, 20, 0, m_width, m_height, 0, 
        XCB_WINDOW_CLASS_INPUT_OUTPUT, 
          m_screen->root_visual, mask, values);
      
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

    const int atoms_count = sizeof(atom_names)/sizeof(char *);
    xcb_intern_atom_cookie_t atom_cookie[atoms_count];
    xcb_atom_t atom_list[atoms_count];
    xcb_intern_atom_reply_t *atom_reply;

    for (int i = 0; i < atoms_count; i++)
        atom_cookie[i] = xcb_intern_atom(m_connection, 0, 
          strlen(atom_names[i]), atom_names[i]);

    for (int i = 0; i < atoms_count; i++) {
        atom_reply = xcb_intern_atom_reply(m_connection, atom_cookie[i], NULL);
        if (!atom_reply)
            return;
        atom_list[i] = atom_reply->atom;
        free(atom_reply);
    }

    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window,
      atom_list[NET_WM_WINDOW_TYPE], XCB_ATOM_ATOM, 32, 1, 
        &atom_list[NET_WM_WINDOW_TYPE_DOCK]);
    xcb_change_property(m_connection, XCB_PROP_MODE_APPEND,  m_window,
      atom_list[NET_WM_STATE], XCB_ATOM_ATOM, 32, 2, 
        &atom_list[NET_WM_STATE_STICKY]);
    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window,
      atom_list[NET_WM_DESKTOP], XCB_ATOM_CARDINAL, 32, 1, 
        (const int []){ -1 } );
    
    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window,
      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, 7, "comfybar");
    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, 
      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8 ,strlen("comfybar"), "comfybar");

    m_created = true;
  }
  
  bool Bar::isCreated() const {
    return m_created;
  }

  void Bar::m_handleMouseButtonPress(xcb_generic_event_t* event) {
    xcb_button_press_event_t* btn_event = (xcb_button_press_event_t*) event;
    std::cout << "Mouse button pressed (" << btn_event->detail << "): " 
      << btn_event->event_x << ":" << btn_event->event_y << std::endl;
    m_running = false;
  }
  
  void Bar::m_handleExposureEvent(xcb_generic_event_t* event) {
  }

  void Bar::show() {
    xcb_map_window(m_connection, m_window);
    xcb_flush(m_connection);

    m_running = true;

    xcb_generic_event_t* event;
    while(m_running) {
      event = xcb_wait_for_event(m_connection);
      if(!event) break; // an error occured
      switch(event->response_type & ~0x80) {
        case XCB_EXPOSE:
          m_handleExposureEvent(event);
          break;
        case XCB_BUTTON_PRESS:
          m_handleMouseButtonPress(event);
          break;
      }
      free(event);
    }
  }

  Bar::~Bar() {
    if(m_connection) {
      xcb_flush(m_connection);
      xcb_disconnect(m_connection);
    }
  }
}