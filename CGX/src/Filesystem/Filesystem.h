#pragma once

#include <string>
#include <vector>

static void Split(std::vector<std::string>& tolkens, const std::string& path, const char& mark);

void Get_Directory_From_Path(std::string& directory, const std::string& path);
void Get_Filename_From_Path(std::string& filename, const std::string& path);
