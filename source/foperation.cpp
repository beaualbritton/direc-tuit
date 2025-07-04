#include "foperation.hpp"
#include "user_config.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

bool deletePath(std::filesystem::path pPath) {
  // Checking if valid path, proceeding
  if (!fs::exists(pPath))
    return false;
  // Checking if path is a directory, different operations follow
  if (fs::is_directory(pPath)) {
    // So removal_all() won't return a boolean like filesystem::remove, instead
    // it returns number of paths deleted, so if nothing is deleted (<= 0)
    // assume false
    return fs::remove_all(pPath) > 0;
  } else {
    // Not a directory, so use std::filesystem::remove()
    return fs::remove(pPath);
  }
}

void renamePath(std::filesystem::path pPath, string renameString) {
  // Retrieve parent directory for filesysystem::rename
  fs::path parentPath;
  if (pPath.has_parent_path())
    parentPath = pPath.parent_path();
  // std::filesystem syntax '/' joins string to filename
  fs::rename(pPath, parentPath / (renameString + getExtension(pPath)));

  // TODO: add some handling so that directories cant be renamed with file
  // extensions & vice versa
}

std::string getExtension(std::filesystem::path pPath) {
  std::string returnString, pathString;
  // Checking if directory
  if (fs::is_directory(pPath)) {
    return "/";
  }
  // File extension
  pathString = pPath.extension().string();
  // TODO: perhaps more robust checking for hidden files like .bashrc
  return pathString;
}

void createFile(std::filesystem::path parentPath, std::string fileName,
                bool dirFlag) {
  // Flagging directory from parameters for separation of logic. Directories use
  // filesystem::create_directory. Files use standard ofstream
  if (dirFlag)
    fs::create_directory(parentPath / fileName);
  else {
    std::ofstream outFile(parentPath / fileName);
    outFile.close();
  }
}

void copyFile(std::filesystem::path copyPath) {
  // call user-config
  // TODO: sanitize/validate a little here
  copyToConfig(copyPath);
}

void pasteFile(std::filesystem::path parentDir) {
  // get current copy path
  fs::path currentCopyPath = getCopiedPath();

  // Evaluate path for copy options (vanilla for files, recursive for dirs, so
  // subdirectories are also copied)
  //
  if (fs::is_directory(currentCopyPath)) {
    // See https://en.cppreference.com/w/cpp/filesystem/copy.html if this is
    // confusing
    fs::copy(currentCopyPath, parentDir / currentCopyPath.filename(),
             fs::copy_options::recursive);
  }
  // Otherwise a file. Default is overwite. TODO: if this fucks anything up
  // change it
  else {
    fs::copy(currentCopyPath, parentDir, fs::copy_options::overwrite_existing);
  }
}
std::string getPermissionString(std::filesystem::path pPath) {

  // This is a bit tricky. Windows 'Philosophy' doesn't support permissions for
  // 'groups' or 'others'. Need to only add owner info in this case. Not using
  // fs::permissions for this reason. Instead, getting manually.
  // https://en.cppreference.com/w/cpp/filesystem/perms.html
  //
  fs::file_status pPathStatus;
  string permissionString = "";

  // Thank you cpp doc dwelling lambda wizards for this one
  // See docs above. Just wanna make this more explicit
  std::function<void(char, fs::perms)> addPermission =
      [pPath, &permissionString](char op, fs::perms perm) {
        // I lied. Gotta use permissions for proper bit mask with &
        permissionString +=
            ((perm & fs::status(pPath).permissions()) == fs::perms::none ? '-'
                                                                         : op);
      };
#ifdef _WIN32
  addPermission('r', fs::perms::owner_read);
  addPermission('w', fs::perms::owner_write);
  addPermission('x', fs::perms::owner_exec);
  // Every other perm on windows doesnt exist
  for (int i = 0; i < 6; ++i)
    permissionString += '-';
#else
  addPermission('r', fs::perms::owner_read);
  addPermission('w', fs::perms::owner_write);
  addPermission('x', fs::perms::owner_exec);
  addPermission('r', fs::perms::group_read);
  addPermission('w', fs::perms::group_write);
  addPermission('x', fs::perms::group_exec);
  addPermission('r', fs::perms::others_read);
  addPermission('w', fs::perms::others_write);
  addPermission('x', fs::perms::others_exec);

#endif // _WIN32

  return permissionString;
}

std::string getPreviewString(std::filesystem::path pPath) {
  std::string line;
  std::ostringstream previewString;
  if (fs::is_directory(pPath))
    return "this is a dir! you shouldn't see this..";

  std::ifstream previewFile(pPath);
  if (!previewFile.is_open())
    return "nil";

  int count = 0;
  while (std::getline(previewFile, line) && count < 90) {
    previewString << line << "\n";
    ++count;
  }
  return previewString.str();
}
