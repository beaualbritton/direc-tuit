#include "user_config.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
// incoming c++ dub
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace ftxui;
using namespace std;
// using std::string;
// using std::filesystem::create_directory; using std::filesystem::exists;
using std::filesystem::path;
const path dirPath = ".config";

bool checkConfig()
{
  bool pathExists = exists(dirPath);
  if (pathExists)
  {
    loadDirectory();
  }
  else
  {
    initDirectory();
  }
  return pathExists;
}
// TODO: this just checks to make sure .toml file is in /.produc-tuity
void loadDirectory() {}
void initDirectory()
{
  // create directory
  create_directory(dirPath);
  // create user_config file
  // filesystem lib magic
  const path filePath = dirPath / "userconfig.toml";
  ofstream configFile(filePath);
  if (configFile)
  {
    configFile << "\n";
    configFile.close();
  }
  else
  {
    cout << "problem loading config";
  }
}
// TODO: parsing .toml for user_name
// TODO: implement toml++ parsing
string getUsername();
