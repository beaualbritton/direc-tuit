#include <filesystem>
using std::string, std::shared_ptr, std::make_shared;

// Returns T/F if path is deleted using std::filesystem::remove
bool deletePath(std::filesystem::path pPath);

void renamePath(std::filesystem::path pPath, string renameString);
