#include "file_explorer.hpp"
#include "../foperation.hpp"
#include "../user_config.hpp"
#include "../window.hpp"
#include "popup.hpp"
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
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
      Renderer([pPath] { return text((pPath.filename().string())); });

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

  Component bodyContainerEvents = CatchEvent(bodyContainer, [=](Event event) {
    if (event == Event::Character('n')) {
      // Create behavior (for focused path)
      shared_ptr<string> newFileString = make_shared<string>();
      shared_ptr<bool> dirFlag = make_shared<bool>();
      auto createLambda = [=] {
        createFile(pPath, *newFileString, *dirFlag);
        WindowRender::instance().getScreen().Post(
            [=] { populate(pContainer, pPath); });
        *modalBool = false;
      };
      popupContainer->DetachAllChildren();
      *currentPopupContent = newOptionPopUp(
          modalBool.get(), pPath, newFileString, createLambda, dirFlag.get());
      popupContainer->Add(*currentPopupContent);
      *modalBool = true;
      return true;
    }
    if (event == Event::Character('p')) {
      pasteFile(pPath);
      WindowRender::instance().getScreen().Post(
          [=] { populate(pContainer, pPath); });
      return true;
    }
    return false;
  });

  horizontalContainer->Add(Renderer(pinnedContainer, [pinnedContainer] {
    return pinnedContainer->Render() |
           size(WIDTH, EQUAL, EXPLORER_WIDTH * 0.1875);
  }));
  horizontalContainer->Add(Renderer([] { return separator(); }));
  horizontalContainer->Add(Renderer(bodyContainerEvents, [bodyContainer] {
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
      // Pin behavior
      if (event == Event::Character('f') && is_directory(iterPath)) {
        auto popupLambda = [=] {
          pinDirectory(iterPath);
          pinnedContainer->DetachAllChildren();
          getUserPinned(pinnedContainer, pContainer);
          *modalBool = false;
        };
        popupContainer->DetachAllChildren();
        *currentPopupContent =
            horizontalPopup("Pin directory: " + iterPath.filename().string(),
                            modalBool.get(), popupLambda);
        popupContainer->Add(*currentPopupContent);
        // Open the modal (ftxui popup)
        *modalBool = true;
        return true;
      }
      // Delete behavior
      if (event == Event::Character('d')) {
        auto refreshLambda = [=] {
          bool deleted = deletePath(iterPath);
          if (deleted) {
            WindowRender::instance().getScreen().Post(
                [=] { populate(pContainer, iterPath.parent_path()); });
            *modalBool = false;
          }
        };
        popupContainer->DetachAllChildren();
        *currentPopupContent =
            deleteOptionPopUp(modalBool.get(), iterPath, refreshLambda);
        popupContainer->Add(*currentPopupContent);
        *modalBool = true;
        return true;
      }
      // Rename behavior
      if (event == Event::Character('r')) {
        shared_ptr<string> renameString = make_shared<string>();
        auto renameLambda = [=] {
          renamePath(iterPath, *renameString);
          WindowRender::instance().getScreen().Post(
              [=] { populate(pContainer, iterPath.parent_path()); });
          *modalBool = false;
        };
        popupContainer->DetachAllChildren();
        *currentPopupContent = renameOptionPopUp(modalBool.get(), iterPath,
                                                 renameString, renameLambda);
        popupContainer->Add(*currentPopupContent);
        *modalBool = true;
        return true;
      }

      // Copy/yank behavior
      if (event == Event::Character('y')) {
        copyFile(iterPath);
        return true;
      }
      // Paste recent yank

      return false;
    });
    string textString = (iterPath.filename().string());
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

  Component submitContainer = Container::Horizontal({});

  Component submitButton, cancelButton;
  // Submits current directory to .config for prefered directory to store info.

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
      ": " + (hDir.filename().string()),
      [hDir, fileContainer] {
        WindowRender::instance().getScreen().Post(
            [=] { populate(fileContainer, hDir); });
      },
      ButtonOption::Ascii());

  std::vector<path> pinnedDirs = getPinnedDirs();

  globalContainer->Add(homeButton);
  for (path p : pinnedDirs) {
    Component currentPinButton = Button(
        ": " + p.filename().string(),
        [p, fileContainer] {
          WindowRender::instance().getScreen().Post(
              [=] { populate(fileContainer, p); });
        },
        ButtonOption::Ascii());
    Component buttonWrapper = CatchEvent(currentPinButton, [=](Event event) {
      // Delete *Pinned* behavior
      if (event == Event::Character('d')) { /*call pinned dir */
        auto popupLambda = [=] {
          deletePin(p);
          pContainer->DetachAllChildren();
          getUserPinned(pContainer, fileContainer);
          *modalBool = false;
        };
        popupContainer->DetachAllChildren();
        *currentPopupContent =
            horizontalPopup("Delete pin: " + p.filename().string(),
                            modalBool.get(), popupLambda);
        popupContainer->Add(*currentPopupContent);
        *modalBool = true;

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
// TODO: test this a little more -- sometimes causes issues on my Windows
// machine
path homeDir() {
#ifdef _WIN32
  return std::getenv("USERPROFILE");
#else
  return std::getenv("HOME");
#endif
}
