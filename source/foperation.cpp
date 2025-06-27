#include "foperation.hpp"
#include <filesystem>

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
  fs::rename(pPath, parentPath / renameString);

  // TODO: add some handling so that directories cant be renamed with file
  // extensions & vice versa
}
