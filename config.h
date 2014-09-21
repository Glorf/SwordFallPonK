#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <map>

/// \brief Store and parser of configuration file.
class Config
{
public:
    Config();
    ~Config();
    void loadConfig();
    ///< Loads config file to memory.

    std::vector<std::string> getConfigValues(std::string option);
    ///< @return a vector of values associated to specified option

    void saveValues(std::string option, std::vector<std::string> values);
    ///< Changes/saves new values into memory

private:
    void saveConfig();
    std::map<std::string, std::vector<std::string> > storage;
    std::string configDir;
};

#endif // CONFIG_H
