#include <filesystem>
#include <vector>
using std::string, std::shared_ptr, std::make_shared;

bool checkConfig();

// initializes .produc-tuity directory with empty userconfig file
void initDirectory();

// loads existing .produc-tuity directory
void loadDirectory();

// pretty explicit
shared_ptr<string> getUsername();
void setUsername(string *username);

void setPreferredPath(std::filesystem::path pPath);
// Pin directory behaviors
void pinDirectory(std::filesystem::path pDir);
std::vector<std::filesystem::path> getPinnedDirs();
void deletePin(std::filesystem::path pDir);

// Copies to .config/userconfig.toml
void copyToConfig(std::filesystem::path copyFile);
std::filesystem::path getCopiedPath();

// Adds up to 5 most recently visited directories
void addToRecents(std::filesystem::path pPath);
std::vector<std::filesystem::path> getRecentList();

// Responsible for [O]pen with functionality. Currently in progress.
bool externalEditorSet();
void setPathToEditor(std::filesystem::path editorPath);
std::filesystem::path getExternalEditorPath();
