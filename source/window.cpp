#include "window.hpp"
#include "dialogue_window.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
using namespace ftxui;
using std::string;

void WindowRender::setupWindow() {
  // TODO: change this based on config
  setWindow(userDialogueWindow());
}
/*
 * not currently in use as setWindow allows for nested active windows
 */
void WindowRender::run() {
  //  rootWindow = CatchEvent(rootWindow, [&](Event event) {
  //    if (event == Event::Character('q')) {
  //      screen.ExitLoopClosure()();
  //      return true;
  //    }
  //    return false;
  //  });
  //  screen.Loop(rootWindow);
}

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

// Element WindowRender::getStartMenu(bool userSet)
//{
//   Element startWindow;
//   if (!userSet)
//   {
//     startWindow = userDialogueWindow();
//   }
//   else
//   {
//     startWindow = introWindow();
//   }
//   return startWindow;
// }
