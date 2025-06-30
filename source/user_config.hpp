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
void pinDirectory(std::filesystem::path pDir);
std::vector<std::filesystem::path> getPinnedDirs();
void deletePin(std::filesystem::path pDir);

// Copies to .config/userconfig.toml
void copyToConfig(std::filesystem::path copyFile);
std::filesystem::path getCopiedPath();
