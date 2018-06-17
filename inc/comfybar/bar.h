#ifndef _COMFYBAR_BAR_H_
#define _COMFYBAR_BAR_H_

#include <xcb/xcb.h>
#include <vector>
#include <comfybar/area.h>
#include <comfybar/config.h>

namespace comfybar {
  class Bar {
  public:
    Bar(int, char** );
    void create();
    void show();
    bool isCreated() const;
    ~Bar();

  private:
    // helper functions
    void m_handleMouseButtonPress(xcb_generic_event_t* );
    void m_handleExposureEvent(xcb_generic_event_t* );

    // all the areas in the bar => every module has its own area
    std::vector<comfybar::Area> m_areas;

    // config class
    Config m_config;

    // xcb stuff
    xcb_connection_t* m_connection;
    xcb_screen_t* m_screen;
    xcb_window_t m_window;
    int m_screen_cnt, m_width, m_height;
    bool m_running;
    bool m_created;
  };
}

#endif