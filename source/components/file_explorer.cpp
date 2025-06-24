#include "file_explorer.hpp"
#include "../user_config.hpp"
#include "../window.hpp"
#include "popup.hpp"
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
shared_ptr<bool> modalBool;
Component popupContainer;
shared_ptr<Component> currentPopupContent;

Component fileExplorer() {
  modalBool = make_shared<bool>(false);
  currentPopupContent = make_shared<Component>();
  popupContainer = Container::Horizontal({});
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
  currentPopupContent = make_shared<Component>();
  *currentPopupContent =
      Renderer([=] { return text("nil"); }); // Empty initially

  // Create the popup that will render the current popup content
  popupContainer->Add(*currentPopupContent);
  // Store the popup content pointer globally so it can be updated

  return Modal(popupContainer, modalBool.get())(explorer);
}

void populate(shared_ptr<ComponentBase> pContainer, const path &pPath) {
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

  auto *screen = ScreenInteractive::Active();
  Component wrapContainer = Container::Horizontal({});
  for (auto const &entry : std::filesystem::directory_iterator{pPath}) {
    const path iterPath = entry.path();
    string label = is_directory(iterPath) ? "" : "";
    *sharedString = label;
    Component fileButton = Button(
        is_directory(iterPath) ? "" : "",
        [iterPath, pContainer, screen] {
          if (is_directory(iterPath)) {
            WindowRender::instance().getScreen().Post(
                [=] { populate(pContainer, iterPath); });
          }
        },
        ButtonOption::Ascii());

    Component catchFileEvents = CatchEvent(fileButton, [=](Event event) {
      if (event == Event::Character('p')) {
        // Build the new popup
        auto popupLambda = [=] {
          pinDirectory(iterPath);
          pinnedContainer->DetachAllChildren();
          getUserPinned(pinnedContainer, pContainer);
          *modalBool = false;
        };
        popupContainer->DetachAllChildren();
        *currentPopupContent =
            horizontalPopup("Pin directory: ", modalBool.get(), popupLambda);
        popupContainer->Add(*currentPopupContent);
        // Open the modal
        *modalBool = true;
        return true;
      }
      return false;
    });
    string textString = string(iterPath.filename());
    Component buttonText = Renderer([textString, iterPath] {
      return text(iterPath.empty() ? "Empty " : textString) |
             size(WIDTH, EQUAL, ((EXPLORER_WIDTH * 0.825) - 4) / 5);
    });

    Component vContainer = Container::Vertical({catchFileEvents, buttonText});

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

  // Get current screen

  // Quits current operation (q behavior)
  cancelButton =
      Button("Cancel.", WindowRender::instance().getScreen().ExitLoopClosure(),
             ButtonOption::Ascii());

  submitButton = Button(
      "Submit.",
      [=] {
        setPreferredPath(pPath);
        WindowRender::instance().getScreen().ExitLoopClosure()();
      },
      ButtonOption::Ascii());
  submitContainer->Add(submitButton);
  submitContainer->Add(cancelButton);
  pContainer->Add(Renderer(submitContainer, [submitContainer] {
    return submitContainer->Render() |
           size(HEIGHT, EQUAL, EXPLORER_HEIGHT * 0.125f);
  }));
}

/*
  Two main parts of pinnedContainer. Global pins. Recents.

  Global: Pinned directories set by the user
  Recents: Recently used files, folders (last five are stored in
  .config/explorer.toml)

*/
void getUserPinned(Component pContainer, Component fileContainer) {

  Component globalContainer = Container::Vertical({});
  Component recentContainer = Container::Vertical({});
  path hDir = homeDir();
  Component homeButton = Button(
      ": " + string(hDir.filename()),
      [hDir, fileContainer] { populate(fileContainer, hDir); },
      ButtonOption::Ascii());

  std::vector<path> pinnedDirs = getPinnedDirs();

  globalContainer->Add(homeButton);
  for (path p : pinnedDirs) {
    Component currentPinButton = Button(
        ": " + p.filename().string(),
        [p, fileContainer] { populate(fileContainer, p); },
        ButtonOption::Ascii());
    Component buttonWrapper = CatchEvent(currentPinButton, [=](Event event) {
      if (event == Event::Character('d')) { /*call pinned dir */
        deletePin(p);
        pContainer->DetachAllChildren();
        getUserPinned(pContainer, fileContainer);
        return true;
      }
      return false;
    });
    globalContainer->Add(buttonWrapper);
  }

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
