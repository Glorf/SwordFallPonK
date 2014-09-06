#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <map>

class Config
{
public:
    Config();
    ~Config();
    void loadConfig();
    std::vector<std::string> getConfigValues(std::string option);
    void saveValues(std::string option, std::vector<std::string> values);
    //void refreshConfig();

private:
    void saveConfig();

    std::map<std::string, std::vector<std::string> > storage;
    std::string configDir;
};

#endif // CONFIG_H
