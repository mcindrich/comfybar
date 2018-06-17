#ifndef _COMFYBAR_CONFIG_H_
#define _COMFYBAR_CONFIG_H_

#include <vector>
#include <comfybar/bar-exceptions.h>

namespace comfybar {
  // value = 100, str = "Hello";
  struct ConfigValue {
    ConfigValue():name(nullptr), value(nullptr) {

    }
    char* name;
    char* value;
  };

  // [section], [module/text1]
  struct ConfigSection {
    ConfigSection():name(nullptr) {
      
    }
    char* name;
    std::vector<ConfigValue> values;
  };

  class Config {
  public:
    Config();
    void loadFromFile(const char* );
    void loadFromString(char* );

    char* getGlobalValue(const char* ) const;
    //                     sec_name,    var_name
    char* getSectionValue(const char*, const char* );
    ~Config();
  private:
    int m_line_number;
    void m_parseSectionName(char*, char** );
    void m_parseValue(char*, char** );
    
    ConfigSection* m_getSection(const char*);

    std::vector<ConfigValue> m_global_values;
    std::vector<ConfigSection> m_sections;
  };
}

#endif