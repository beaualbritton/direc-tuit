#include "user_config.hpp"
// incoming c++ dub
#include <filesystem>

#include <fstream>
#include <iostream>
#include <toml++/impl/parse_error.hpp>
#include <toml++/impl/parser.hpp>
#include <toml++/toml.hpp>
using namespace std;
// using std::string;
// using std::filesystem::create_directory; using std::filesystem::exists;
using std::filesystem::path, std::shared_ptr, std::make_shared;

const path dirPath = ".config";
const path filePath = dirPath / "userconfig.toml";

bool checkConfig() {
  bool pathExists = exists(dirPath);
  if (pathExists) {
    // loadDirectory();
  } else {
    initDirectory();
  }
  return pathExists;
}

void initDirectory() {
  // create directory
  create_directory(dirPath);
  // create user_config file
  // filesystem lib magic
  ofstream configFile(filePath);
  if (configFile) {
    configFile << "";
    configFile.close();
  } else {
    cout << "problem loading config";
  }
}
// Username functionality not in use currently. Disregard. Read for
// understanding of toml++ syntax (if curious)
//  Uses toml++ parsing for efficient&readable reads/writes to config
shared_ptr<string> getUsername() {
  shared_ptr<string> username = make_shared<string>();
  //  string *username = nullptr;
  try {
    toml::table user_table = toml::parse_file(filePath.string());
    auto value = user_table["username"];
    // checking types so no crazy shit happens
    if (value.is_string()) {
      // toml++ is weird because as_string() returns value<std::string>*. not
      // std::string* ? weird asf.. so doing some shotty dereferencing here to
      // get the actual value of the value<std::string> (devs pick a better name
      // im having a stroke just typing this ) that's why we having to check if
      // it's a string to begin with. otherwise we'd be dereferencing *username
      // (which is nullptr at this point) if you change this... beware of
      // segfaults ^_^
      username = make_unique<string>(*value.as_string());
    } else {
      username = make_unique<string>("null");
    }

  }
  // TODO: comprehensive catch, not just generic print.
  catch (toml::parse_error &err) {
    cout << "parsing failed: " << err;
  }
  return username;
}

// sets username using toml parsing
void setUsername(string *username) {
  try {
    toml::table user_table = toml::parse_file(filePath.string());

    user_table.insert_or_assign("username", *username);
    ofstream configFile(filePath);
    if (configFile) {
      configFile << user_table;
    }
  }
  // TODO: again more comprehensive catches
  catch (const toml::parse_error &err) {
    cout << "assigning username failed: " << err;
  }
}
void pinDirectory(filesystem::path pDir) {
  try {
    toml::table path_table = toml::parse_file(filePath.string());

    toml::array *pin_list = path_table["pinned"].as_array();
    if (!pin_list) {
      path_table.insert_or_assign("pinned", toml::array{});
      pin_list = path_table["pinned"].as_array();
    }
    // checking for duplicates
    for (auto &index : *pin_list) {
      // Found in pins already. Don't do anything
      if (index.is_string() && index.value<string>() == pDir.string())
        return;
    }
    pin_list->push_back(pDir.string());

    ofstream configFile(filePath);
    if (configFile) {
      configFile << path_table;
    }
  }
  // TODO: again more comprehensive catches
  catch (const toml::parse_error &err) {
    cout << "assigning path failed: " << err;
  }
}

void deletePin(filesystem::path pDir) {
  // Deletes selected path from pin list in .config/user_config.toml
  //
  try {
    toml::table config_table = toml::parse_file(filePath.string());
    toml::array *pin_array = config_table["pinned"].as_array();
    if (!pin_array) {
      // No pins to delete
      return;
    }
    // Toml++ provides no erase(value) function. Includes appending values.
    // kinda weird best reasoning i found was an issue on the gh repo from like
    // 3 years ago. apparentlly toml::array arr[i] would level all of mumbai if
    // the repo owner tried to implement it.. Anyways because bro is lazy we are
    // lazily looping over this array

    int pinCount = 0;
    while (pinCount < pin_array->size()) {
      // look at how they massacred my boy..... shit's almost as ugly as rust
      // code
      if ((*pin_array)[pinCount].value<std::string>() == pDir.string()) {
        // gotta watch out here, indexes can be skipped due to erase() shifting
        // indices
        pin_array->erase(pin_array->begin() + pinCount);
      } else
        // only move up when not erasing
        ++pinCount;
    }
    ofstream configFile(filePath);
    if (configFile) {
      configFile << config_table;
    }
  } catch (const toml::parse_error &err) {
    cout << "deleting pin failed: " << err;
  }
}

vector<filesystem::path> getPinnedDirs() {
  vector<filesystem::path> pinDirVector;
  try {
    toml::table path_table = toml::parse_file(filePath.string());
    toml::array *path_list = path_table["pinned"].as_array();
    if (!path_list)
      // return empty vector
      return vector<filesystem::path>();

    for (auto &pin : *path_list) {
      auto *strPin = pin.as_string();
      if (strPin != nullptr) {
        pinDirVector.emplace_back(strPin->get());
      }
    }

  } catch (const toml::parse_error &err) {
    cout << "assigning path failed: " << err;
  }
  return pinDirVector;
}

void copyToConfig(std::filesystem::path copyFile) {
  try {
    toml::table config_table = toml::parse_file(filePath.string());
    config_table.insert_or_assign("current_copy", copyFile.string());
    ofstream configFile(filePath);
    if (configFile) {
      configFile << config_table;
    }

  } catch (const toml::parse_error &err) {
    cout << "copying file failed:" << err;
  }
}

// Don't be fooled! This is for pasting functionality
std::filesystem::path getCopiedPath() {
  try {
    toml::table config_table = toml::parse_file(filePath.string());
    // Using auto because toml::node* is tricky. compiler save me
    auto pathValue = config_table["current_copy"];
    if (!pathValue) {
      // Throwing an error at *this* line. don't want to return anything except
      // a valid path, so throw an error to be caught
      throw toml::parse_error("missing copy file",
                              toml::source_position{219, 0});
    }
    if (pathValue.is_string()) {
      // Parsing right type
      std::string pathString = pathValue.as_string()->get();
      return std::filesystem::path(pathString);
    }

  } catch (const toml::parse_error &err) {
    cout << "retrieving copied file failed:" << err;
  }
  return std::filesystem::path("");
}

void addToRecents(std::filesystem::path pPath) {
  try {
    toml::table config_table = toml::parse_file(filePath.string());
    toml::array *recentsArray = config_table["recents"].as_array();
    if (!recentsArray) {
      config_table.insert_or_assign("recents", toml::array{});
      recentsArray = config_table["recents"].as_array();
    }
    // Check if recents is bigger than 5 paths.
    int maxRecentSize = 5;
    if (recentsArray->size() >= maxRecentSize) {
      // Remove the least recent
      recentsArray->erase(recentsArray->begin());
    }
    // checking for duplicates
    for (auto &index : *recentsArray) {
      // reusing this from pinDirectory() ... ugly
      if (index.is_string() && index.value<string>() == pPath.string())
        return;
    }
    recentsArray->push_back(pPath.string());
    ofstream configFile(filePath);
    if (configFile) {
      configFile << config_table;
    }
  } catch (const toml::parse_error &err) {
    cout << "error adding recent folder:" << err;
  }
}
std::vector<std::filesystem::path> getRecentList() {
  std::vector<path> recentsVec;
  try {
    toml::table config_table = toml::parse_file(filePath.string());
    toml::array *recentsArray = config_table["recents"].as_array();
    if (!recentsArray) {
      return std::vector<path>();
    };
    for (auto &recent : *recentsArray) {
      auto *strRecent = recent.as_string();
      if (strRecent != nullptr) {
        recentsVec.emplace_back(strRecent->get());
      }
    }
  } catch (const toml::parse_error &err) {
    cout << "error retrieving recents" << err;
  }
  return recentsVec;
}
bool externalEditorSet() {
  bool isSet = false;
  try {
    toml::table config_table = toml::parse_file(filePath.string());
    auto editorPath = config_table["editor"];
    if (editorPath)
      isSet = true;
  } catch (const toml::parse_error &err) {
    cout << "error checking for editor" << err;
  }
  return isSet;
}

void setPathToEditor(std::filesystem::path editorPath) {
  try {
    toml::table config_table = toml::parse_file(filePath.string());
    // if (std::filesystem::exists(editorPath))
    config_table.insert_or_assign("editor", editorPath.string());
    ofstream configFile(filePath);
    if (configFile) {
      configFile << config_table;
    }

  } catch (const toml::parse_error &err) {
    cout << "copying file failed:" << err;
  }
}

std::filesystem::path getExternalEditorPath() {
  std::filesystem::path editorPath = "";
  try {
    toml::table config_table = toml::parse_file(filePath.string());
    auto pathValue = config_table["editor"];
    if (pathValue && pathValue.is_string()) {
      std::string pathString = pathValue.as_string()->get();
      editorPath = std::filesystem::path(pathString);
    }

  } catch (const toml::parse_error &err) {
    cout << "error retrieving editor path :" << err;
  }
  return editorPath;
}
