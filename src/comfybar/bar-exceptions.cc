#include <comfybar/bar-exceptions.h>
#include <cstring>
#include <iostream>

namespace comfybar {
  // base class
  BarException::BarException(const char* msg) {
    int len = strlen(msg);
    m_message = new char[len + 1];
    memcpy(m_message, msg, len);
    m_message[len] = 0;
  }

  char* BarException::getMessage() const {
    return m_message;
  }

  void BarException::printMessage() const {
    std::cout << "[Exception]:  " << m_message << std::endl;
  }

  BarException::~BarException() {
    delete[] m_message;
  }

  // config ex
  ConfigException::ConfigException(const char* msg, int line_num)
    :BarException(msg) {
    m_line_number = line_num;
  }

  void ConfigException::printMessage() const {
    std::cout << "[Config Exception at line " << m_line_number << "]: " <<
      "[" << getMessage() << "]" << std::endl;
  }

  int ConfigException::getLineNumber() const {
    return m_line_number;
  }
}