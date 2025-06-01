#include "user_config.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
// incoming c++ dub
#include <filesystem>

#include <fstream>
#include <iostream>
#include <toml++/toml.hpp>

using namespace ftxui;
using namespace std;
// using std::string;
// using std::filesystem::create_directory; using std::filesystem::exists;
using std::filesystem::path, std::shared_ptr, std::make_shared;

const path dirPath = ".config";
const path filePath = dirPath / "userconfig.toml";

bool checkConfig()
{
  bool pathExists = exists(dirPath);
  if (pathExists)
  {
    // loadDirectory();
  }
  else
  {
    initDirectory();
  }
  return pathExists;
}

void initDirectory()
{
  // create directory
  create_directory(dirPath);
  // create user_config file
  // filesystem lib magic
  ofstream configFile(filePath);
  if (configFile)
  {
    configFile << "";
    configFile.close();
  }
  else
  {
    cout << "problem loading config";
  }
}
// Uses toml++ parsing for efficient&readable reads/writes to config
shared_ptr<string> getUsername()
{
  shared_ptr<string> username = make_shared<string>();
  //  string *username = nullptr;
  try
  {
    toml::table user_table = toml::parse_file(filePath.string());
    auto value = user_table["username"];
    // checking types so no crazy shit happens
    if (value.is_string())
    {
      // toml++ is weird because as_string() returns value<std::string>*. not
      // std::string* ? weird asf.. so doing some shotty dereferencing here to
      // get the actual value of the value<std::string> (devs pick a better name
      // im having a stroke just typing this ) that's why we having to check if
      // it's a string to begin with. otherwise we'd be dereferencing *username
      // (which is nullptr at this point) if you change this... beware of
      // segfaults ^_^
      username = make_unique<string>(*value.as_string());
    }
    else
    {
      username = make_unique<string>("null");
    }

  }
  // TODO: comprehensive catch, not just generic print.
  catch (toml::parse_error &err)
  {
    cout << "parsing failed: " << err;
  }
  return username;
}

// sets username using toml parsing
void setUsername(string *username)
{
  try
  {
    toml::table user_table = toml::parse_file(filePath.string());

    user_table.insert_or_assign("username", *username);
    ofstream configFile(filePath);
    if (configFile)
    {
      configFile << user_table;
    }
  }
  // TODO: again more comprehensive catches
  catch (const toml::parse_error &err)
  {
    cout << "assigning username failed: " << err;
  }
}
