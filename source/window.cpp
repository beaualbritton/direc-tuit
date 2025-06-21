#include "window.hpp"
#include "components/file_explorer.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
using namespace ftxui;
using std::string;

void WindowRender::setupWindow() { this->setWindow(fileExplorer()); }

/*
 * not currently in use as setWindow allows for nested active windows
 */
// void WindowRender::run() {
//   rootWindow = CatchEvent(rootWindow, [&](Event event) {
//     if (event == Event::Character('q')) {
//       screen.ExitLoopClosure()();
//       return true;
//     }
//     return false;
//   });
//   screen.Loop(rootWindow);
//}

void WindowRender::setWindow(Component cWindow) {
  rootWindow = cWindow;
  rootWindow = CatchEvent(rootWindow, [&](Event event) {
    if (event == Event::Character('q')) {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });
  screen.Loop(rootWindow);
}

ScreenInteractive &WindowRender::getScreen() { return screen; };
