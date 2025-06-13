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

void populate(shared_ptr<ComponentBase> pContainer, const path &pPath) {
  pContainer->DetachAllChildren();

  Component parentDirButton = Button("../", [pPath, pContainer] {
    if (pPath.has_parent_path()) {
      populate(pContainer, pPath.parent_path());
    }
  });
  pContainer->Add(parentDirButton);
  for (auto const &entry : std::filesystem::directory_iterator{pPath}) {
    const path iterPath = entry.path();
    string label;
    if (is_directory(iterPath))
      label = ": ";
    else
      label = ": ";
    Component fileButton =
        Button(label + string(iterPath), [iterPath, pContainer] {
          if (is_directory(iterPath)) {
            populate(pContainer, iterPath);
          }
        });
    pContainer->Add(fileButton);
  }
}
