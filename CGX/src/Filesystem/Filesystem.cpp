#include "Filesystem.h"

void Split(std::vector<std::string>& tolkens, const std::string& path, const char& mark)
{
    const int MAX_TOLKENS = 10; 
    tolkens.reserve(MAX_TOLKENS); 
    std::string temp = ""; 

    for (int i = 0; i < path.size(); ++i) 
    {
        if (path[i] == mark) 
        {
            temp += mark;
            tolkens.push_back(temp); 
            temp = ""; 
            continue; 
        }

        temp += path[i]; 
    }

    tolkens.push_back(temp);
}

void Get_Directory_From_Path(std::string& directory, const std::string& path)
{
    std::vector<std::string> tolkens; 

    Split(tolkens, path, '/'); 

    for (int i = 0; i < tolkens.size() - 1; ++i) 
    {
        directory += tolkens[i]; 
    }
}

void Get_Filename_From_Path(std::string& filename, const std::string& path)
{
    std::vector<std::string> tolkens;

    Split(tolkens, path, '/');

    filename = tolkens[tolkens.size() - 1];
}
