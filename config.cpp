#include "config.h"
#include <dirent.h>
#include <fstream>
#include <iostream>

Config::Config()
{}

Config::~Config()
{}

void Config::loadConfig()
{
    storage.clear();

    std::ifstream fileStream;

    fileStream.open("./config/settings.ponk");

    if(!fileStream.good())
        return;

    while(!fileStream.eof())
    {
        std::string line;
        std::vector<std::string> translatedLine;

        //line decryption
        std::getline(fileStream, line);

        if(line.size()==0)
            break;

        std::string::iterator it;
        std::string temporaryCommandContainer;
        bool nameEnclosure = false;
        for(it=line.begin();it!=line.end();it++)
        {
            if(*it == '\"')
            {
                nameEnclosure = !nameEnclosure;
            }
            else if(*it == ' ' && !nameEnclosure)
            {
                translatedLine.push_back(temporaryCommandContainer);
                temporaryCommandContainer = "";
            }
            else
            {
                temporaryCommandContainer += *it;
            }
        }

        if(temporaryCommandContainer.size()>0)
        {
            translatedLine.push_back(temporaryCommandContainer);
            temporaryCommandContainer = "";
        }

        std::vector<std::string> v;
        for(unsigned i=1; i<translatedLine.size();i++)
        {
            v.push_back(translatedLine.at(i));
        }

        storage[translatedLine.at(0)] = v;

    }
}

std::vector<std::string> Config::getConfigValues(std::string option)
{
    return storage[option];
}

void Config::saveValues(std::string option, std::vector<std::string> values)
{
    storage[option] = values;

    saveConfig();
}

void Config::saveConfig()
{
    std::ofstream fileStream;
    fileStream.open("./config/settings.ponk");

    if(!fileStream.good())
        return;

    std::map<std::string, std::vector<std::string>>::iterator it;
    for(it=storage.begin();it!=storage.end();it++)
    {
        fileStream<<it->first;

        std::vector<std::string>::iterator it2;
        for(it2=it->second.begin();it2!=it->second.end();it2++)
        {
            fileStream<<" ";
            fileStream<<*it2;
        }

        fileStream<<"\n";
    }
}
