#include "window.hpp"
#include "dialogue_window.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
using namespace ftxui;
using std::string;

WindowRender::WindowRender() {}

void WindowRender::setupWindow()
{
  Element title, content;

  title = text("produc-tuity");
  content = vbox({paragraph("welcome to produc-tuity")});
  // currentWindow = userDialogueWindow(); // window(title | bold, content);
  windowComponent = userDialogueWindow();
}

void WindowRender::run()
{
  windowComponent = CatchEvent(windowComponent, [&](Event event) {
    if (event == Event::Character('q'))
    {
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });
  screen.Loop(windowComponent);
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
