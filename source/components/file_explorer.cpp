#include <filesystem>
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>
#include <vector>

using namespace ftxui;

using std::filesystem::path, std::shared_ptr, std::make_shared, std::vector,
    std::string;

Component fileExplorer() {
  // Navigates to ~/
  path currentPath = std::filesystem::current_path();
  Component homeButton = Button(string(currentPath), [] {});

  Component homeButton1 = Button(string(currentPath), [] {});
  // Get current directory and sub-directories & files
  // container for fileButtons
  Component fileContainer = Container::Vertical({homeButton, homeButton1});

  vector<string> pathNames = vector<string>();
  for (auto const &entry : std::filesystem::directory_iterator{currentPath}) {
    // issues using fileContainer->Add() in this loop, so storing these string
    // values in a vector
    pathNames.push_back(string(entry.path()));
  }
  for (int i = 0; i < pathNames.size(); ++i) {
    Component fileButton = Button(pathNames[i], [] {});
    fileContainer->Add(fileButton);
  }

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
