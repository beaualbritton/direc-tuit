#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;
using std::string, std::shared_ptr, std::make_shared;

// returns t/f based on if there is an initialized .produc-tuity directory in
// this repo
bool checkConfig();

// initializes .produc-tuity directory with empty userconfig file
void initDirectory();

// loads existing .produc-tuity directory
void loadDirectory();

// pretty explicit
shared_ptr<string> getUsername();
void setUsername(string *username);

void setPreferredPath(std::filesystem::path pPath);
void pinDirectory(std::filesystem::path pDir);
