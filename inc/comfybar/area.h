#ifndef _COMFYBAR_AREA_H_
#define _COMFYBAR_AREA_H_

#include <xcb/xcb.h>

namespace comfybar {
  class Area {
  public:
    Area();
    ~Area();
  private:
    int m_topr, m_topl, m_bottomr, m_bottoml;
  };
}

#endif