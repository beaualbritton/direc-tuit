#include "file_explorer.hpp"
#include <filesystem>
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <memory>
#include <vector>

using namespace ftxui;

using std::filesystem::path, std::filesystem::is_directory, std::shared_ptr,
    std::make_shared, std::vector, std::string;

Component fileExplorer() {
  // Navigates to ~/
  path currentPath = std::filesystem::current_path();
  Component homeButton = Button(string("home"), [] {});

  Component homeButton1 = Button(string(currentPath), [] {});
  // Get current directory and sub-directories & files
  // container for fileButtons

  // Using Componentbase rather than Component for pointer management
  // (share_ptr<Component> wont Render in the Renderer fn)
  shared_ptr<ComponentBase> fileContainer = Container::Vertical({});

  populate(fileContainer, currentPath);

  // TODO: ADD OPERATIONS (make, delete, open)

  /*
   TODO: ADD:
   * -Sidebar with pinned dirs (Home, Desktop, Documents, etc.)
   * -Text that displays current directory name
   * -Use existing directory or make new? (new function/popup)
   * -pointerSelect button
   *  Input box for typing
   *  Keybinds for file operations (create, delete, open.)
   *  Some more robust error handling (if directory shit is weird)
   */

  Component explorer = Renderer(fileContainer, [=] {
    return window(text("file explorer"),
                  vbox({
                      (fileContainer->Render() | // TODO: proper screen ratios
                       size(ftxui::WIDTH, ftxui::EQUAL, 80) |
                       size(ftxui::HEIGHT, ftxui::EQUAL, 60)),
                  })) |
           center | vcenter;
  });
  return explorer;
}
// TODO: instead of populate populating THE ENTIRE container, just have a nested
// container or vector (for separate tabs/sidebars that will be added)
void populate(shared_ptr<ComponentBase> pContainer, const path &pPath) {
  pContainer->DetachAllChildren();

  Component parentDirButton = Button("../", [pPath, pContainer] {
    if (pPath.has_parent_path() && is_directory(pPath.parent_path())) {
      populate(pContainer, pPath.parent_path());
    }
  });
  pContainer->Add(parentDirButton);
  shared_ptr<string> sharedString = make_shared<string>();
  for (auto const &entry : std::filesystem::directory_iterator{pPath}) {
    const path iterPath = entry.path();
    string label = "";
    if (is_directory(iterPath))
      label += ": "; //+ iterPath.filename().string();
    else
      label += ": ";
    label += iterPath.filename().string();
    *sharedString = label;
    Component fileButton = Button(*sharedString, [iterPath, pContainer] {
      if (is_directory(iterPath)) {
        populate(pContainer, iterPath);
      }
    });
    pContainer->Add(fileButton);
  }
}
