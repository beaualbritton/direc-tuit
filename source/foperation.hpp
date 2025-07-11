#include <filesystem>
using std::string, std::shared_ptr, std::make_shared;

// Returns T/F if path is deleted using std::filesystem::remove
bool deletePath(std::filesystem::path pPath);

void renamePath(std::filesystem::path pPath, string renameString);
// Helper function for rename functionality
std::string getExtension(std::filesystem::path pPath);

void createFile(std::filesystem::path parentPath, std::string fileName,
                bool dirFlag);
// Copy file. This does not call filesystem::copy. Instead, it flags a file to
// copy in .config Why? Allows pasting outside multiple sessions. I plan to
// include 'clip-board' functionality
void copyFile(std::filesystem::path copyPath);

void pasteFile(std::filesystem::path parentPath);
// helper for paste&rename
std::string getStamp();

std::string getPermissionString(std::filesystem::path pPath);
std::string getPreviewString(std::filesystem::path pPath);

void openWith(std::filesystem::path pPath);
