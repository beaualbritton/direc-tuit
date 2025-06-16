#include "file_explorer.hpp"
#include <filesystem>
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <iostream>
#include <memory>
#include <vector>

using namespace ftxui;

using std::filesystem::path, std::filesystem::is_directory, std::shared_ptr,
    std::make_shared, std::vector, std::string;

Component fileExplorer() {
  // Navigates to ~/
  path currentPath = std::filesystem::current_path();
  // Get current directory and sub-directories & files
  // container for fileButtons

  // Using Componentbase rather than Component for pointer management
  // (share_ptr<Component> wont Render in the Renderer fn)
  Component fileContainer = Container::Vertical({});

  shared_ptr<ComponentBase> baseContainer =
      Container::Horizontal({fileContainer});
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

  Component explorer = Renderer(baseContainer, [=] {
    return window(text("file explorer"),
                  vbox({
                      (baseContainer->Render() | // TODO: proper screen ratios
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

  shared_ptr<string> sharedString = make_shared<string>();
  Component currentDirLabel =
      Renderer([pPath] { return text(string(pPath.filename())); });

  string label;
  int wrapCount = 0;

  Component parentDirButton = Button(
      "../",
      [pPath, pContainer] {
        if (pPath.has_parent_path()) {
          populate(pContainer, pPath.parent_path());
        }
      },
      ButtonOption::Ascii());

  Component currentDirContainer =
      Container::Horizontal({parentDirButton, currentDirLabel});

  pContainer->Add(currentDirContainer);
  pContainer->Add(Renderer([] { return separator(); }));

  Component horizontalContainer = Container::Horizontal({});
  Component pinnedContainer = Container::Vertical({});
  Component bodyContainer = Container::Vertical({});

  horizontalContainer->Add(Renderer(pinnedContainer, [pinnedContainer] {
    return pinnedContainer->Render() | size(ftxui::WIDTH, ftxui::EQUAL, 10);
  }));
  horizontalContainer->Add(Renderer([] { return separator(); }));
  horizontalContainer->Add(Renderer(bodyContainer, [bodyContainer] {
    return bodyContainer->Render() | size(WIDTH, EQUAL, 70);
  }));

  getUserPinned(pinnedContainer, pContainer);

  Component wrapContainer = Container::Horizontal({});
  for (auto const &entry : std::filesystem::directory_iterator{pPath}) {
    const path iterPath = entry.path();
    label = is_directory(iterPath) ? "" : "";
    *sharedString = label;
    Component fileButton = Button(
        *sharedString,
        [iterPath, pContainer] {
          if (is_directory(iterPath)) {
            populate(pContainer, iterPath);
          }
        },
        ButtonOption::Ascii());
    Component buttonText = Renderer([iterPath] {
      return text(string(iterPath.filename())) |
             size(ftxui::WIDTH, ftxui::EQUAL, 10);
    });

    Component vContainer = Container::Vertical({fileButton, buttonText});

    Component renderSeparator = Renderer([] { return separator(); });
    wrapContainer->Add(vContainer);
    wrapContainer->Add(renderSeparator);
    ++wrapCount;
    if (wrapCount >= 4) {
      bodyContainer->Add(wrapContainer);
      wrapCount = 0;
      wrapContainer = Container::Horizontal({});
    }
  }

  if (wrapCount > 0) {
    bodyContainer->Add(wrapContainer);
  }

  pContainer->Add(horizontalContainer);

  // TODO: add submit bar
}

/*
  Two main parts of pinnedContainer. Global pins. Recents.

  Global: Pinned directories set by the user
  Recents: Recently used files, folders (last five are stored in
  .config/explorer.toml)

*/
void getUserPinned(Component pContainer, Component fileContainer) {
  // TODO: fetch info from .config
  //
  Component globalContainer = Container::Vertical({});
  Component recentContainer = Container::Vertical({});
  path hDir = homeDir();
  Component homeButton = Button(
      "", [hDir, fileContainer] { populate(fileContainer, hDir); },
      ButtonOption::Ascii());

  globalContainer->Add(homeButton);
  recentContainer->Add(Renderer([] { return text("none"); }));
  pContainer->Add(globalContainer);
  pContainer->Add(recentContainer);
}
path homeDir() {
#ifdef _WIN32
  return std::getenv("USERPROFILE");
#else
  return std::getenv("HOME");
#endif
}
