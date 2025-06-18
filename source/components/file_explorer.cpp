#include "file_explorer.hpp"
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <memory>
using namespace ftxui;
using std::filesystem::path, std::filesystem::is_directory, std::shared_ptr,
    std::make_shared, std::string;

// Some constants for screen size.
auto terminalSize = Terminal::Size();
const int TERM_WIDTH = terminalSize.dimx;
const int TERM_HEIGHT = terminalSize.dimy;
const int EXPLORER_WIDTH = TERM_WIDTH * 0.85;
const int EXPLORER_HEIGHT = TERM_HEIGHT * 0.75;

Component fileExplorer() {
  path currentPath = std::filesystem::current_path();
  // Get current directory and sub-directories & files
  // container for fileButtons

  // Using Componentbase rather than Component for pointer management
  // (share_ptr<Component> wont Render in the Renderer fn)
  Component fileContainer = Container::Vertical({});

  shared_ptr<ComponentBase> baseContainer =
      Container::Horizontal({fileContainer});
  populate(fileContainer, currentPath);

  /*
   TODO: ADD:
   * -Sidebar with pinned dirs (Home, Desktop, Documents, etc.)
   * -Use existing directory or make new? (new function/popup)
   * -pointerSelect button
   *  Input box for typing
   *  Keybinds for file operations (create, delete, open.)
   *  Some more robust error handling (if directory shit is weird)
   */

  Component explorer = Renderer(baseContainer, [=] {
    return window(text("file explorer"),
                  vbox({
                      (baseContainer->Render() |
                       size(WIDTH, EQUAL, EXPLORER_WIDTH) |
                       size(HEIGHT, EQUAL, EXPLORER_HEIGHT)),
                  })) |
           center | vcenter;
  });
  return explorer;
}

void populate(shared_ptr<ComponentBase> pContainer, const path &pPath) {
  /*
   TODO:
   FIX CRASH ON MOUSE EVENT
   I've boiled it down to this line here,
   This function works properly with keyboard input. No issues, thoroughly
   tested. However, the story is much different for mouse clicks. I'm fairly
   certain the mouse click event is not handled in the same way that buttons
   are.

   Commenting out DetachAllChildren breaks functionality (stacks explorer
   frames) but does not crash when mouse is clicked.
   */

  pContainer->DetachAllChildren();

  shared_ptr<string> sharedString = make_shared<string>();
  Component currentDirLabel =
      Renderer([pPath] { return text(string(pPath.filename())); });

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
  int scrollVal = 0;
  Component bodyContainer = Container::Vertical({});

  horizontalContainer->Add(Renderer(pinnedContainer, [pinnedContainer] {
    return pinnedContainer->Render() |
           size(WIDTH, EQUAL, EXPLORER_WIDTH * 0.1875);
  }));
  horizontalContainer->Add(Renderer([] { return separator(); }));
  horizontalContainer->Add(Renderer(bodyContainer, [bodyContainer] {
    return bodyContainer->Render() |
           size(WIDTH, EQUAL, EXPLORER_WIDTH * 0.825) |
           size(HEIGHT, EQUAL, EXPLORER_HEIGHT) | frame | vscroll_indicator;
  }));

  getUserPinned(pinnedContainer, pContainer);

  Component wrapContainer = Container::Horizontal({});
  for (auto const &entry : std::filesystem::directory_iterator{pPath}) {
    const path iterPath = entry.path();
    string label = is_directory(iterPath) ? "" : "";
    *sharedString = label;
    Component fileButton = Button(
        is_directory(iterPath) ? "" : "",
        [iterPath, pContainer] {
          if (is_directory(iterPath)) {
            populate(pContainer, iterPath);
          }
        },
        ButtonOption::Ascii());
    string textString = string(iterPath.filename());
    Component buttonText = Renderer([textString, iterPath] {
      return text(iterPath.empty() ? "Empty " : textString) |
             size(WIDTH, EQUAL, ((EXPLORER_WIDTH * 0.825) - 4) / 5);
    });

    Component vContainer = Container::Vertical({fileButton, buttonText});

    Component renderSeparator = Renderer([] { return separator(); });
    wrapContainer->Add(vContainer);
    wrapContainer->Add(renderSeparator);
    ++wrapCount;
    if (wrapCount >= 5) {
      bodyContainer->Add(wrapContainer);
      wrapCount = 0;
      wrapContainer = Container::Horizontal({});
    }
  }

  if (wrapCount > 0) {
    bodyContainer->Add(wrapContainer);
  }

  pContainer->Add(horizontalContainer);

  pContainer->Add(Renderer([] { return separator(); }));

  // TODO: add submit bar
  Component submitContainer = Container::Horizontal({});

  Component submitButton, cancelButton;
  // Submits current directory to .config for prefered directory to store info.
  submitButton = Button("Submit.", [] {}, ButtonOption::Ascii());

  // Get current screen
  auto *activeScreen = ScreenInteractive::Active();

  // Quits current operation (q behavior)
  cancelButton =
      Button("Cancel.", activeScreen->ExitLoopClosure(), ButtonOption::Ascii());
  submitContainer->Add(submitButton);
  submitContainer->Add(cancelButton);
  pContainer->Add(Renderer(submitContainer, [submitContainer] {
    return submitContainer->Render() |
           size(HEIGHT, EQUAL, EXPLORER_HEIGHT * 0.15f);
  }));
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
      ": " + string(hDir.filename()),
      [hDir, fileContainer] { populate(fileContainer, hDir); },
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
