#include "foperation.hpp"
#include "iostream"
#include "user_config.hpp"
#include "window.hpp"
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <ftxui/component/screen_interactive.hpp>
#include <functional>
#include <sstream>
#include <string>

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
  if (!fs::exists(currentCopyPath))
    return;
  fs::path currentCopyFileName = currentCopyPath.filename();
  // Checking if path exists in parent dir
  if (fs::exists(parentDir / currentCopyFileName))
    currentCopyFileName += "copied @ " + getStamp() +getExtension(currentCopyFileName);

  // Evaluate path for copy options (vanilla for files, recursive for dirs, so
  // subdirectories are also copied)
  //
  if (fs::is_directory(currentCopyPath)) {
    // See https://en.cppreference.com/w/cpp/filesystem/copy.html if this is
    // confusing
    fs::copy(currentCopyPath, parentDir / currentCopyFileName,
             fs::copy_options::recursive);
  }
  // Otherwise a file. Default is overwite. TODO: if this fucks anything up
  // change it
  else {
    fs::copy(currentCopyPath, parentDir / currentCopyFileName, fs::copy_options::update_existing);
  }
}
// Helper function for paste & later rename
std::string getStamp() {
  using std::ostringstream, std::chrono::system_clock;
  // i'd rather use c++20 chrono::format but i'm worried about
  // cross-compatability cobbled this together with hopes & dreams
  // https://en.cppreference.com/w/cpp/chrono.html
  // https://en.cppreference.com/w/cpp/chrono/c/time_t.html
  system_clock::time_point tp = system_clock::now();
  std::time_t tp_t = system_clock::to_time_t(tp);
  std::tm *tpLocal = std::localtime(&tp_t);

  ostringstream oString;
  oString << std::put_time(tpLocal, "%y%m%d_%H%M%S"); // 1984 ahh formatting
  return oString.str();
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
  // this is ugly but i didn't figure i could loop thru (based on fs::perms not
  // being iterable), plus it looks like this on official c++ reference docs...
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
void openWith(std::filesystem::path pPath) {
  auto &screen = WindowRender::instance().getScreen();
  std::filesystem::path editorPath = getExternalEditorPath();
  std::string command =
      editorPath.string() + " " + std::filesystem::absolute(pPath).string();
  // This took hours of scouring the FTXUI docs.. Recent updates to their github
  // pages documentation has left things quite sparse. Legit read the
  // implementation and scratched my head for 90 mins before figuring out that I
  // can pass a 'closure' lambda to WithRestoredIO (pretty much equivalent to
  // std::atexit[](){} but takes closure lambda instead of void*. i tried
  // implementing this too but no luck...) Now, editor can be ran inside
  // FTXUI... and then once, quit, FTXUI will continue it's session...
  // magicks!!!! If curious, please read.
  // https://arthursonzogni.github.io/FTXUI/with__restored__io_8cpp_source.html
  auto runEditorWithRestoredIO = screen.WithRestoredIO([=] {
    std::cout << "Executing: " << command << std::endl;
    return std::system(command.c_str());
  });
  screen.ExitLoopClosure();
  runEditorWithRestoredIO();
}
