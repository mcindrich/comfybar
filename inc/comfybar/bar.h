#ifndef _COMFYBAR_BAR_H_
#define _COMFYBAR_BAR_H_

#include <X11/Xlib.h>
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
    void m_handleMouseButtonPress(XEvent* );
    void m_handleExposureEvent(XEvent* );

    // all the areas in the bar => every module has its own area
    std::vector<comfybar::Area> m_areas;

    // config class
    Config m_config;

    // Xlib stuff
    Display* m_display;
    Window m_window;
    XSetWindowAttributes m_wattributes;

    // bar properties
    int m_screen, m_width, m_height;
    bool m_running, m_created, m_bottom;
  };
}

#endif