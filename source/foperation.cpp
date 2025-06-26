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
