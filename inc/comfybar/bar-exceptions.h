#ifndef _COMFYBAR_BAR_EXCEPTIONS_H_
#define _COMFYBAR_BAR_EXCEPTIONS_H_

namespace comfybar {
  class BarException {
  public:
    BarException(const char* );
    virtual void printMessage() const;
    char* getMessage() const;
    ~BarException();
  private:
    char* m_message;
  };

  class ConfigException : public BarException {
  public:
    ConfigException(const char* msg, int );
    void printMessage() const;
    int getLineNumber() const;
  private:
    int m_line_number;
  };

}

#endif