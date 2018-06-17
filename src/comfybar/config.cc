#include <comfybar/config.h>
#include <fstream>
#include <iostream>
#include <cstring>

namespace comfybar {
   static inline bool configIsToken(char c) {
    return c == '\n' || c == '[' || c == ' ' || c == '\t' || c == '=';
  }

  static inline bool configIsEscapeChar(char c) {
    return c == '\n' || c == '\t' || c == ' ';
  }

  Config::Config():m_line_number(1) {
    
  }

  char* Config::getGlobalValue(const char* name) const {
    for(int i = 0; i < m_global_values.size(); i++) {
      if(strcmp(m_global_values[i].name, name) == 0) {
        return m_global_values[i].value;
      }
    }
    return nullptr;
  }

  ConfigSection* Config::m_getSection(const char* sn) {
    for(int i = 0; i < m_sections.size(); i++) {
      if(strcmp(m_sections[i].name, sn) == 0) {
        return &m_sections[i];
      }
    }
    return nullptr;
  }

  char* Config::getSectionValue(const char* sec_name, const char* name) {
    ConfigSection* section_ptr = m_getSection(sec_name);
    if(section_ptr) {
      for(int i = 0; i < section_ptr->values.size(); i++) {
        if(strcmp(section_ptr->values[i].name, name) == 0) {
          return section_ptr->values[i].value;
        }
      }
    }
    return nullptr;
  }

  void Config::loadFromString(char* content) {
    char *cptr = content, *lptr = cptr; // last pointer ==> last token place
    for(; *cptr; cptr++) {
      if(configIsToken(*cptr)) {
        if(*cptr == '[') {
          m_parseSectionName(lptr, &cptr);
        } else if(cptr - lptr) {
          m_parseValue(lptr, &cptr);
        } else {
          if(*cptr == '\n') { // still count the line number
            m_line_number++;
          }
        }
        lptr = cptr + 1;
      }
    }
  }

  void Config::loadFromFile(const char* file_name) {
    std::ifstream file(file_name);
    if(file.is_open()) {
      // read the file and parse it
      std::string str((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
      loadFromString((char*) str.data());
      file.close();
    } else {
      throw ConfigException("Unable to open file.", 0);
    }
  }

  void Config::m_parseSectionName(char* lptr, char** cptr) {
    // skip starting spaces and newlines
    ConfigSection section;
    size_t section_name_len;

    for((*cptr)++; **cptr && configIsEscapeChar(**cptr) && **cptr != ']';
      (*cptr)++) {
      if(**cptr == '\n') {
        throw 
          ConfigException(
            "Newline character reached while parsing section name.", 
            m_line_number);
      }
    }

    if(!**cptr || **cptr == ']') {
      // error ==> reached the EOF
      throw ConfigException("Blank section name.", m_line_number);
    }

    // get the name ==> go until the space char is reach
    lptr = *cptr;
    for((*cptr)++; **cptr && !configIsEscapeChar(**cptr) && **cptr != ']'; 
      (*cptr)++);
    
    if(!**cptr || **cptr == '\n') {
      throw ConfigException("Invalid section definition.", m_line_number);
    }
    section_name_len = *cptr - lptr;
    //std::cout << "Section len: " << *cptr - lptr << std::endl;
    section.name = new char[section_name_len + 1];
    memcpy(section.name, lptr, section_name_len);
    section.name[section_name_len] = 0;

    // successfully created a section
    m_sections.push_back(section);

    // go untile the ']' token
    for(; **cptr && configIsEscapeChar(**cptr) && **cptr != ']'; 
      (*cptr)++) {
      if(**cptr == '\n') {
        throw 
          ConfigException(
            "Newline character reached while parsing section name.",
            m_line_number);
      }
    }
  }
  
  void Config::m_parseValue(char* lptr, char** cptr) {
    if(**cptr == '\n') {
      // error ==> delimiter for the var name and the '=' sign is invalid
      //throw ConfigException("Invalid config value definition.", m_line_number);
    }
    ConfigValue value;
    size_t name_len, value_len;
    char* last = lptr;

    for(lptr++; *lptr && !configIsEscapeChar(*lptr) && *lptr != '='; lptr++);

    if(*lptr == '\n') {
      // error var __newline__ = ...
      throw ConfigException("Newline detected while parsing value assignment.",
        m_line_number);
    }

    name_len = lptr - last;
    value.name = new char[name_len + 1];
    memcpy(value.name, last, name_len);
    value.name[name_len] = 0;

    // go on until '=' and if another letter or number was found ==> report err
    for(; *lptr && configIsEscapeChar(*lptr) && *lptr != '='; lptr++) {
      if(*lptr == '\n') {
        throw
          ConfigException(
            "Newline character reached while parsing config value.", 
            m_line_number);
      }
    }
    if(*lptr != '=') {
      throw ConfigException("No assignment operator.", m_line_number);
    }

    // go on through all the spaces
    for(lptr++; *lptr && configIsEscapeChar(*lptr); lptr++) {
      if(*lptr == '\n') {
        throw
          ConfigException(
            "Newline character reached while parsing config value.", 
            m_line_number);
      }
    }
    // get the value and finish
    last = lptr;
    for(lptr++; *lptr && !configIsEscapeChar(*lptr); lptr++);
    value_len = lptr - last;
    value.value = new char[value_len + 1];
    memcpy(value.value, last, value_len);
    value.value[value_len] = 0;

    // add the value to the appropriate container
    if(m_sections.size() == 0) {
      m_global_values.push_back(value);
    } else {
      m_sections.at(m_sections.size()-1).values.push_back(value);
    }
    // change the content pointer
    *cptr = lptr;
  }

  static inline void configDeleteValues(std::vector<ConfigValue>& vec) {
    for(auto it = vec.begin(); it != vec.end(); it++) {
      if(it->name)
        delete[] it->name;
      if(it->value)
        delete[] it->value;
    }
  }

  Config::~Config() {
    // free all sections and all values
    for(auto it = m_sections.begin(); it != m_sections.end(); it++) {
      delete[] it->name;
      configDeleteValues(it->values);
    }
    configDeleteValues(m_global_values);
  }
}